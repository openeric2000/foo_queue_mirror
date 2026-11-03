#include "stdafx.h"
#include "config.h"
#include "resource.h"
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

namespace qpl {
	// {7B2E9A14-3D5C-4E8F-A6B1-0C4D8E2F9A53}
	static constexpr GUID guid_cfg_name = { 0x7b2e9a14, 0x3d5c, 0x4e8f, { 0xa6, 0xb1, 0x0c, 0x4d, 0x8e, 0x2f, 0x9a, 0x53 } };
	// 預設對應名為「佇列清單」的清單（配合 Nocturne for foobar2000 主題）；沒有同名清單時不會有任何動作，清空即停用
	static constexpr const char* default_playlist_name = "佇列清單";
	// 主題英文版的佇列清單叫「Queue」：名稱還是預設值時，找不到「佇列清單」就改找它
	static constexpr const char* default_playlist_name_en = "Queue";
	cfg_string cfgPlaylistName(guid_cfg_name, default_playlist_name);

	// 設定頁欄位裡顯示的名稱：還是預設值時，顯示實際會對應到的那個清單的名字（中文版主題是「佇列清單」、
	// 英文版主題沒有「佇列清單」但有「Queue」，就顯示 Queue），兩邊都不用改設定就看得到對的名稱。
	// 只影響畫面；實際對應哪個清單仍由 findMirrorPlaylist 決定
	pfc::string8 pageNameFor(const pfc::string8& configured) {
		if (strcmp(configured.c_str(), default_playlist_name) != 0) return configured;
		auto pm = playlist_manager::get();
		if (pm->find_playlist(default_playlist_name) == SIZE_MAX && pm->find_playlist(default_playlist_name_en) != SIZE_MAX)
			return pfc::string8(default_playlist_name_en);
		return configured;
	}

	size_t findMirrorPlaylist() {
		pfc::string8 name = cfgPlaylistName.get();
		if (name.is_empty()) return SIZE_MAX;

		auto pm = playlist_manager::get();
		const size_t idx = pm->find_playlist(name.c_str());
		if (idx != SIZE_MAX || strcmp(name.c_str(), default_playlist_name) != 0) return idx;
		return pm->find_playlist(default_playlist_name_en);
	}
}

namespace {
	class CPreferences : public CDialogImpl<CPreferences>, public preferences_page_instance {
	public:
		CPreferences(preferences_page_callback::ptr callback) : m_callback(callback) {}

		enum { IDD = IDD_PREFERENCES };

		t_uint32 get_state() override {
			t_uint32 state = preferences_state::resettable | preferences_state::dark_mode_supported;
			if (hasChanged()) state |= preferences_state::changed;
			return state;
		}
		void apply() override {
			m_shown = readEdit();
			qpl::cfgPlaylistName = m_shown.c_str();
			qpl::requestSync();
			m_callback->on_state_changed();
		}
		void reset() override {
			uSetDlgItemText(*this, IDC_PLAYLIST_NAME, qpl::pageNameFor(pfc::string8(qpl::default_playlist_name)).c_str());
			m_callback->on_state_changed();
		}

		BEGIN_MSG_MAP_EX(CPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_PLAYLIST_NAME, EN_CHANGE, OnEditChange)
		END_MSG_MAP()

	private:
		BOOL OnInitDialog(CWindow, LPARAM) {
			m_dark.AddDialogWithControls(*this);
			m_shown = qpl::pageNameFor(qpl::cfgPlaylistName.get());
			uSetDlgItemText(*this, IDC_PLAYLIST_NAME, m_shown.c_str());
			return FALSE;
		}
		void OnEditChange(UINT, int, CWindow) { m_callback->on_state_changed(); }

		pfc::string8 readEdit() {
			pfc::string8 s;
			uGetDlgItemText(*this, IDC_PLAYLIST_NAME, s);
			return s;
		}
		// 跟「一開始顯示的名稱」比，不是跟設定值比：英文版顯示的是 Queue，設定值還是預設的「佇列清單」，不能一打開就算有變更
		bool hasChanged() { return strcmp(readEdit().c_str(), m_shown.c_str()) != 0; }

		const preferences_page_callback::ptr m_callback;
		pfc::string8 m_shown;
		fb2k::CDarkModeHooks m_dark;
	};

	class preferences_page_queue : public preferences_page_impl<CPreferences> {
	public:
		const char* get_name() override { return "Queue Mirror"; }
		GUID get_guid() override {
			// {2E6F1C83-9B47-4A0D-8C35-71D2E5B8F960}
			return GUID{ 0x2e6f1c83, 0x9b47, 0x4a0d, { 0x8c, 0x35, 0x71, 0xd2, 0xe5, 0xb8, 0xf9, 0x60 } };
		}
		GUID get_parent_guid() override { return guid_tools; }
	};

	static preferences_page_factory_t<preferences_page_queue> g_prefsFactory;
}
