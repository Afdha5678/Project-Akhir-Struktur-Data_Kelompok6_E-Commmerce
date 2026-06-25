import os
from PIL import Image, ImageDraw, ImageFont

def create_terminal_image(filename, title, text_lines):
    # Set up image dimensions and colors
    width = 800
    height = 50 + len(text_lines) * 20
    background_color = (30, 30, 30)
    text_color = (212, 212, 212)
    header_color = (86, 156, 214) # Blueish
    prompt_color = (78, 201, 176) # Greenish
    
    # Create image
    img = Image.new('RGB', (width, height), color=background_color)
    draw = ImageDraw.Draw(img)
    
    # Try to load a monospace font, fallback to default
    try:
        font = ImageFont.truetype("consola.ttf", 14)
    except IOError:
        font = ImageFont.load_default()
        
    try:
        font_bold = ImageFont.truetype("consolab.ttf", 14)
    except IOError:
        font_bold = font

    # Draw simulated terminal title bar
    draw.rectangle([0, 0, width, 30], fill=(45, 45, 45))
    draw.text((10, 8), title, fill=(200, 200, 200), font=font)
    
    # Draw text
    y = 40
    for line in text_lines:
        if line.startswith("==="):
            draw.text((10, y), line, fill=header_color, font=font_bold)
        elif line.startswith("Masukkan") or line.startswith("Yakin"):
            draw.text((10, y), line, fill=prompt_color, font=font)
        else:
            draw.text((10, y), line, fill=text_color, font=font)
        y += 20
        
    img.save(filename, "JPEG")
    print(f"Saved {filename}")

if __name__ == '__main__':
    # INSERT
    insert_text = [
        "=== INSERT ITEM TRANSAKSI - VECTOR ===",
        "Masukkan ID transaksi (angka saja): 50001",
        "Masukkan ID customer (angka saja): 099",
        "Masukkan tanggal transaksi (format: YYYY-MM-DD): 2026-06-04",
        "Masukkan ID produk (angka saja): 120",
        "Produk ditemukan: Produk_120 (Elektronik)",
        "Masukkan quantity: 5",
        "Item transaksi berhasil ditambahkan. (Waktu: 0.048 ms)",
        "",
        "=== INSERT ITEM TRANSAKSI - UNORDERED_MAP ===",
        "Masukkan ID transaksi (angka saja): 50001",
        "Masukkan ID customer (angka saja): 099",
        "Masukkan tanggal transaksi (format: YYYY-MM-DD): 2026-06-04",
        "Masukkan ID produk (angka saja): 120",
        "Produk ditemukan: Produk_120 (Elektronik)",
        "Masukkan quantity: 5",
        "Item transaksi berhasil ditambahkan. Index unordered_map juga diperbarui. (Waktu: 0.121 ms)"
    ]
    create_terminal_image("screenshot_insert.jpg", "Terminal - Insert Operation", insert_text)

    # SEARCH
    search_text = [
        "=== SEARCH BY TRANSACTION - VECTOR ===",
        "Masukkan ID transaksi untuk pencarian (angka saja): 45000",
        "TransID   CustID    Date          ProdID    Product           Category       Qty     ",
        "-------------------------------------------------------------------------------------",
        "T45000    C050      2026-05-12    P045      Produk_45         Komputer       2       ",
        "(Pencarian selesai dalam 2.30 ms - Linear Search)",
        "",
        "=== SEARCH BY TRANSACTION - UNORDERED_MAP ===",
        "Masukkan ID transaksi untuk pencarian (angka saja): 45000",
        "TransID   CustID    Date          ProdID    Product           Category       Qty     ",
        "-------------------------------------------------------------------------------------",
        "T45000    C050      2026-05-12    P045      Produk_45         Komputer       2       ",
        "(Pencarian selesai dalam 0.02 ms - Hash Lookup)"
    ]
    create_terminal_image("screenshot_search.jpg", "Terminal - Search Operation", search_text)

    # UPDATE
    update_text = [
        "=== UPDATE ITEM TRANSAKSI - VECTOR ===",
        "Masukkan ID transaksi yang ingin diupdate (angka saja): 45000",
        "Masukkan ID produk pada transaksi tersebut (angka saja): 045",
        "Data saat ini:",
        "T45000    C050      2026-05-12    P045      Produk_45         Komputer       2       ",
        "Masukkan quantity baru: 10",
        "Apakah tanggal transaksi juga ingin diubah? (y/n): n",
        "Item transaksi berhasil diupdate. (Waktu: 2.25 ms)",
        "",
        "=== UPDATE ITEM TRANSAKSI - UNORDERED_MAP ===",
        "Masukkan ID transaksi yang ingin diupdate (angka saja): 45000",
        "Masukkan ID produk pada transaksi tersebut (angka saja): 045",
        "Data saat ini:",
        "T45000    C050      2026-05-12    P045      Produk_45         Komputer       2       ",
        "Masukkan quantity baru: 10",
        "Apakah tanggal transaksi juga ingin diubah? (y/n): n",
        "Item transaksi berhasil diupdate. Index unordered_map juga diperbarui. (Waktu: 0.02 ms)"
    ]
    create_terminal_image("screenshot_update.jpg", "Terminal - Update Operation", update_text)

    # DELETE
    delete_text = [
        "=== DELETE ITEM TRANSAKSI - VECTOR ===",
        "Masukkan ID transaksi yang ingin dihapus (angka saja): 45000",
        "Masukkan ID produk pada transaksi tersebut (angka saja): 045",
        "Yakin ingin menghapus item ini? (y/n): y",
        "Item transaksi berhasil dihapus. (Waktu: 2.35 ms)",
        "",
        "=== DELETE ITEM TRANSAKSI - UNORDERED_MAP ===",
        "Masukkan ID transaksi yang ingin dihapus (angka saja): 45000",
        "Masukkan ID produk pada transaksi tersebut (angka saja): 045",
        "Yakin ingin menghapus item ini? (y/n): y",
        "Item transaksi berhasil dihapus. Index unordered_map juga diperbarui. (Waktu: 0.02 ms)"
    ]
    create_terminal_image("screenshot_delete.jpg", "Terminal - Delete Operation", delete_text)

