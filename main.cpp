#include "stdafx.h"
#include <functional>

DECLARE_COMPONENT_VERSION("Queue Playlist", "1.0.0",
	"Mirrors the playback queue into a regular playlist that you create yourself.\n"
	"Enter the playlist name in Preferences > Advanced > Playback > \"Queue playlist name\".\n"
	"One-way sync (queue -> playlist); edits made in the playlist are not written back to the queue.");

VALIDATE_COMPONENT_FILENAME("foo_queue_playlist.dll");

FOOBAR2000_IMPLEMENT_CFG_VAR_DOWNGRADE;

namespace {
	// {5C0B7A53-6E0B-4B6E-9F0D-3B1E6F2F7A41}
	static const GUID guid_name_entry = { 0x5c0b7a53, 0x6e0b, 0x4b6e, { 0x9f, 0x0d, 0x3b, 0x1e, 0x6f, 0x2f, 0x7a, 0x41 } };

	// 不給預設值，使用者自己決定要對應哪個清單
	advconfig_string_factory g_cfgName("Queue playlist name (leave empty to disable)", "queue_playlist.name", guid_name_entry, advconfig_entry::guid_branch_playback, 0, "");

	bool g_syncing = false;
	bool g_pending = false;

	bool sameHandles(const metadb_handle_list& a, const metadb_handle_list& b) {
		if (a.get_size() != b.get_size()) return false;
		for (size_t i = 0; i < a.get_size(); ++i) {
			if (a[i] != b[i]) return false;
		}
		return true;
	}

	void syncNow() {
		if (g_syncing) return;
		pfc::string8 name = g_cfgName.get();
		if (name.is_empty()) return;

		auto pm = playlist_manager::get();
		const size_t idx = pm->find_playlist(name.c_str());
		if (idx == SIZE_MAX) return;
		if (pm->playlist_lock_is_present(idx)) return;

		pfc::list_t<t_playback_queue_item> queue;
		pm->queue_get_contents(queue);
		metadb_handle_list want;
		for (size_t i = 0; i < queue.get_size(); ++i) want.add_item(queue[i].m_handle);

		metadb_handle_list have;
		pm->playlist_get_all_items(idx, have);
		if (sameHandles(have, want)) return;

		g_syncing = true;
		pm->playlist_clear(idx);
		if (want.get_size() > 0) pm->playlist_add_items(idx, want, pfc::bit_array_false());
		g_syncing = false;
	}

	// 合併同一輪的多次通知，並避免在 callback 內直接改動播放清單
	void scheduleSync() {
		if (g_pending) return;
		g_pending = true;
		fb2k::inMainThread([] {
			g_pending = false;
			syncNow();
		});
	}

	class queue_cb : public playback_queue_callback {
	public:
		void on_changed(t_change_origin) override { scheduleSync(); }
	};
	FB2K_SERVICE_FACTORY(queue_cb);

	// 使用者建立或改名清單時也要同步，才不必等到下一次佇列變動
	class pl_watch : public playlist_callback_impl_base {
	public:
		pl_watch() : playlist_callback_impl_base(flag_on_playlist_created | flag_on_playlist_renamed) {}
		void on_playlist_created(t_size, const char*, t_size) override { scheduleSync(); }
		void on_playlist_renamed(t_size, const char*, t_size) override { scheduleSync(); }
	};

	class lifecycle : public initquit {
	public:
		void on_init() override {
			m_watch = std::make_unique<pl_watch>();
			scheduleSync();
		}
		void on_quit() override { m_watch.reset(); }
	private:
		std::unique_ptr<pl_watch> m_watch;
	};
	FB2K_SERVICE_FACTORY(lifecycle);
}
