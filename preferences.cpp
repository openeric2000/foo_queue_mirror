#include "stdafx.h"
#include "config.h"
#include "resource.h"
#include <helpers/atl-misc.h>
#include <helpers/DarkMode.h>

namespace qpl {
	// {7B2E9A14-3D5C-4E8F-A6B1-0C4D8E2F9A53}
	static constexpr GUID guid_cfg_name = { 0x7b2e9a14, 0x3d5c, 0x4e8f, { 0xa6, 0xb1, 0x0c, 0x4d, 0x8e, 0x2f, 0x9a, 0x53 } };
	// 不給預設值，由使用者自行決定要對應哪個清單
	cfg_string cfgPlaylistName(guid_cfg_name, "");
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
			qpl::cfgPlaylistName = readEdit().c_str();
			qpl::requestSync();
			m_callback->on_state_changed();
		}
		void reset() override {
			uSetDlgItemText(*this, IDC_PLAYLIST_NAME, "");
			m_callback->on_state_changed();
		}

		BEGIN_MSG_MAP_EX(CPreferences)
			MSG_WM_INITDIALOG(OnInitDialog)
			COMMAND_HANDLER_EX(IDC_PLAYLIST_NAME, EN_CHANGE, OnEditChange)
		END_MSG_MAP()

	private:
		BOOL OnInitDialog(CWindow, LPARAM) {
			m_dark.AddDialogWithControls(*this);
			uSetDlgItemText(*this, IDC_PLAYLIST_NAME, qpl::cfgPlaylistName.get().c_str());
			return FALSE;
		}
		void OnEditChange(UINT, int, CWindow) { m_callback->on_state_changed(); }

		pfc::string8 readEdit() {
			pfc::string8 s;
			uGetDlgItemText(*this, IDC_PLAYLIST_NAME, s);
			return s;
		}
		bool hasChanged() { return strcmp(readEdit().c_str(), qpl::cfgPlaylistName.get().c_str()) != 0; }

		const preferences_page_callback::ptr m_callback;
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
