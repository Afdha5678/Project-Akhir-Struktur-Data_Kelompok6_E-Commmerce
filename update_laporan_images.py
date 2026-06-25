import os

def update_laporan_with_images():
    with open('Laporan.md', 'r', encoding='utf-8') as f:
        content = f.read()

    # Find the start of Lampiran D and replace the text with image references
    split_marker = "### Lampiran D: Screenshoot Output CRUD (Vector vs Unordered Map)"
    if split_marker in content:
        main_content = content.split(split_marker)[0]
    else:
        main_content = content
        
    with open('Laporan.md', 'w', encoding='utf-8') as f:
        f.write(main_content)
        f.write(split_marker + "\n\n")
        f.write("Berikut adalah tangkapan layar (screenshot) langsung dari program untuk operasi Insert, Search, Update, dan Delete dari kedua implementasi struktur data.\n\n")
        
        f.write("#### 1. Operasi INSERT\n")
        f.write("![Screenshot Insert](screenshot_insert.jpg)\n\n")
        
        f.write("#### 2. Operasi SEARCH\n")
        f.write("![Screenshot Search](screenshot_search.jpg)\n\n")
        
        f.write("#### 3. Operasi UPDATE\n")
        f.write("![Screenshot Update](screenshot_update.jpg)\n\n")
        
        f.write("#### 4. Operasi DELETE\n")
        f.write("![Screenshot Delete](screenshot_delete.jpg)\n\n")

if __name__ == '__main__':
    update_laporan_with_images()
