import os

def rewrite_laporan():
    with open('Laporan.md', 'r', encoding='utf-8') as f:
        content = f.read()
        
    # Replace Table Waktu Pencarian
    old_table_waktu = """**Tabel Waktu Pencarian (Search) berdasarkan Jumlah Data (ms)**
| Jumlah Transaksi | Vector Search Waktu (ms) | Unordered Map Search Waktu (ms) |
|------------------|---------------------------|----------------------------------|
| 1.000            | 0.04                      | 0.01                             |
| 5.000            | 0.21                      | 0.01                             |
| 10.000           | 0.45                      | 0.01                             |
| 50.000           | 2.30                      | 0.02                             |"""
    
    new_table_waktu = "![Tabel Waktu Pencarian](table_waktu.jpg)"
    content = content.replace(old_table_waktu, new_table_waktu)
    
    # Replace Table Memori
    old_table_memori = """**Tabel Penggunaan Memori (MB)**
| Jumlah Transaksi | Memori Vector (MB) | Memori Unordered Map (MB) |
|------------------|---------------------|----------------------------|
| 1.000            | 0.1                 | 0.2                        |
| 5.000            | 0.5                 | 0.8                        |
| 10.000           | 1.0                 | 1.6                        |
| 50.000           | 5.0                 | 8.2                        |"""

    new_table_memori = "![Tabel Penggunaan Memori](table_memori.jpg)"
    content = content.replace(old_table_memori, new_table_memori)
    
    # Extract out Lampiran B Text Block and replace
    start_b = "```text\n===================================================\n   BENCHMARK SISTEM TRANSAKSI"
    end_b = "[Proses Benchmark Selesai]\n```"
    
    if start_b in content and end_b in content:
        start_idx = content.find(start_b)
        end_idx = content.find(end_b) + len(end_b)
        content = content[:start_idx] + "![Screenshot Simulasi Program](terminal_lampiran_b.jpg)" + content[end_idx:]
    
    # Extract out Lampiran C Text Block and replace
    start_c = "```text\n================================================================================================================================\n                                          TABEL PERBANDINGAN SEBELAHAN (BENCHMARK)"
    end_c = "================================================================================================================================\n```"
    
    if start_c in content and end_c in content:
        start_idx = content.find(start_c)
        end_idx = content.find(end_c) + len(end_c)
        content = content[:start_idx] + "![Tabel Benchmark Lengkap](terminal_lampiran_c.jpg)" + content[end_idx:]

    with open('Laporan.md', 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == '__main__':
    rewrite_laporan()
