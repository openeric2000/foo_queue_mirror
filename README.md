# foo_queue_playlist

A tiny foobar2000 (v2, x64) component that mirrors the playback queue into a regular playlist, so you can see what is queued as a tab next to your other playlists.

## Requirements

- [foobar2000](https://www.foobar2000.org/) v2.x, 64-bit (Windows)
- No other component is required.
- Optional: [Queue Viewer](https://marc2k3.github.io/component/queue-viewer/) for editing the queue itself (this component is one-way).

## Usage

1. Install the `.fb2k-component` and restart foobar2000.
2. Create a normal playlist (any name, e.g. `Queue`).
3. Go to **Preferences > Tools > Queue Playlist** and enter the same name. Empty = disabled.

The playlist now follows the queue automatically.

## Notes

- One-way sync (queue -> playlist). Edits in the playlist are not written back to the queue.
- Locked playlists are skipped.
- x64 only.

## Build

The project depends on the [foobar2000 SDK](https://www.foobar2000.org/SDK) (not included). Place this folder next to `foo_sample` inside the SDK's `foobar2000` directory, then build with Visual Studio (C++20, x64, Release).

## License

MIT. The foobar2000 SDK has its own license.

---

# foo_queue_playlist（中文）

極輕量的 foobar2000（v2、x64）元件，把播放佇列（queue）即時映射到一般播放清單，讓你在播放清單 tab 列就能看到目前佇列內容。

## 前置需求

- [foobar2000](https://www.foobar2000.org/) v2.x 64 位元（Windows）
- 不需要安裝其他元件。
- 選用：[Queue Viewer](https://marc2k3.github.io/component/queue-viewer/)，用來直接編輯佇列本身（本元件是單向映射）。

## 使用方式

1. 安裝 `.fb2k-component` 後重啟 foobar2000。
2. 自行新增一個一般播放清單（名稱自訂，例如 `Queue`）。
3. 到 **Preferences > Tools > Queue Playlist**，填入相同名稱。留空即停用。

之後這個清單會自動跟著佇列變動。

## 注意事項

- 單向同步（佇列 → 清單），在清單上的編輯不會回寫佇列。
- 被鎖定的清單會略過。
- 僅支援 x64。

## 建置

需要 [foobar2000 SDK](https://www.foobar2000.org/SDK)（未包含）。把本資料夾放到 SDK 的 `foobar2000` 目錄下（與 `foo_sample` 同層），用 Visual Studio 以 C++20、x64、Release 建置。

## 授權

MIT。foobar2000 SDK 另有其授權條款。
