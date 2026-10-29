#pragma once

namespace qpl {
	extern cfg_string cfgPlaylistName;
	// 映射清單的索引；名稱是預設值時「佇列清單」優先、沒有再找「Queue」；停用或找不到時傳回 SIZE_MAX
	size_t findMirrorPlaylist();
	void requestSync();
}
