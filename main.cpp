#include "stdafx.h"
#include "config.h"
#include <functional>
#include <vector>

DECLARE_COMPONENT_VERSION("Queue Mirror", "1.3.0",
	"Mirrors the playback queue into a regular playlist that you create yourself.\n"
	"By default it uses a playlist named \"佇列清單\" or \"Queue\"; enter another name in Preferences > Tools > Queue Mirror.\n"
	"Sync is queue -> playlist. Deleting items in the playlist also removes them from the queue;\n"
	"other edits (reorder, add) are not written back and will be reverted on the next sync.");

VALIDATE_COMPONENT_FILENAME("foo_queue_mirror.dll");

FOOBAR2000_IMPLEMENT_CFG_VAR_DOWNGRADE;

namespace {
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
		const size_t idx = qpl::findMirrorPlaylist();
		if (idx == SIZE_MAX) return;

		auto pm = playlist_manager::get();
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
		pl_watch() : playlist_callback_impl_base(flag_on_playlist_created | flag_on_playlist_renamed | flag_on_items_removing) {}
		void on_playlist_created(t_size, const char*, t_size) override { scheduleSync(); }
		void on_playlist_renamed(t_size, const char*, t_size) override { scheduleSync(); }

		// 使用者在映射清單中刪除曲目時，同步從真正的佇列移除
		void on_items_removing(t_size playlist, const bit_array& mask, t_size oldCount, t_size) override {
			if (g_syncing) return;
			const size_t idx = qpl::findMirrorPlaylist();
			if (idx == SIZE_MAX || playlist != idx) return;

			// callback 內不能直接動佇列，先記下要移除的位置，延後到主執行緒下一輪處理
			std::vector<size_t> removed;
			for (size_t i = 0; i < oldCount; ++i) {
				if (mask[i]) removed.push_back(i);
			}
			if (removed.empty()) return;

			fb2k::inMainThread([removed, oldCount] {
				auto pm = playlist_manager::get();
				// 清單與佇列不一致時位置無法對應，改用佇列重新同步，避免誤刪
				if (pm->queue_get_count() != oldCount) {
					FB2K_console_formatter() << "Queue Mirror: queue size changed, resyncing instead of removing";
					scheduleSync();
					return;
				}
				pfc::bit_array_bittable mask(oldCount);
				for (size_t i : removed) mask.set(i, true);
				pm->queue_remove_mask(mask);
				FB2K_console_formatter() << "Queue Mirror: removed " << (uint32_t)removed.size() << " item(s) from the queue";
			});
		}
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

void qpl::requestSync() { scheduleSync(); }
