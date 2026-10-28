# foo_queue_mirror

A tiny foobar2000 (v2, x64) component that mirrors the playback queue into a regular playlist, so you can see what is queued as a tab next to your other playlists.

## Usage

1. Install the `.fb2k-component` and restart foobar2000.
2. Create a normal playlist named `佇列清單` (the default name). Nothing happens until a playlist with that name exists.
3. To use another name, go to **Preferences > Tools > Queue Mirror** and enter it. Empty = disabled.

The playlist now follows the queue automatically.

## Notes

- Sync is queue -> playlist. Deleting items in the playlist also removes them from the queue. Other edits (reorder, add) are not written back and are reverted on the next sync.
- Locked playlists are skipped.
- x64 only.

## Build

The project depends on the [foobar2000 SDK](https://www.foobar2000.org/SDK) (not included). Place this folder next to `foo_sample` inside the SDK's `foobar2000` directory, then build with Visual Studio (C++20, x64, Release).

## License

MIT. The foobar2000 SDK has its own license.

---

# foo_queue_mirror（中文）

極輕量的 foobar2000（v2、x64）元件，把播放佇列（queue）即時映射到一般播放清單，讓你在播放清單 tab 列就能看到目前佇列內容。

## 使用方式

1. 安裝 `.fb2k-component` 後重啟 foobar2000。
2. 新增一個名為 `佇列清單` 的一般播放清單（預設名稱）。沒有這個名稱的清單時，元件不會有任何動作。
3. 想用別的名稱，到 **Preferences > Tools > Queue Mirror** 填入該名稱。留空即停用。

之後這個清單會自動跟著佇列變動。

## 注意事項

- 同步方向為佇列 → 清單。在清單中刪除曲目時，也會同步從佇列移除；其他編輯（重排、加入）不會回寫，並會在下次同步時還原。
- 被鎖定的清單會略過。
- 僅支援 x64。

## 建置

需要 [foobar2000 SDK](https://www.foobar2000.org/SDK)（未包含）。把本資料夾放到 SDK 的 `foobar2000` 目錄下（與 `foo_sample` 同層），用 Visual Studio 以 C++20、x64、Release 建置。

## 授權

MIT。foobar2000 SDK 另有其授權條款。
