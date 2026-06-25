import PyPDF2
import sys

def extract_text(pdf_path, txt_path):
    with open(pdf_path, 'rb') as f_in:
        reader = PyPDF2.PdfReader(f_in)
        text = ''
        for i in range(len(reader.pages)):
            text += reader.pages[i].extract_text() + '\n'
            
    with open(txt_path, 'w', encoding='utf-8') as f_out:
        f_out.write(text)
    print(f"Extracted {pdf_path} to {txt_path}")

if __name__ == '__main__':
    pdfs = [
        "Proyek Akhir Struktur Data Genap 2526.docx.pdf",
        "Laporan Progres Proyek Akhir Struktur Data.pdf",
        "Laporan proyek strukdat kel 6 p4.docx.pdf"
    ]
    for pdf in pdfs:
        extract_text(pdf, pdf + ".txt")
