import fitz, os, pytesseract
from PIL import Image

pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'
doc = fitz.open(r'c:/Users/WYC/Desktop/My code/STC15F2K61S2/16届蓝桥杯国赛真题/16届国赛(已优化).pdf')
outdir = r'c:/Users/WYC/Desktop/My code/STC15F2K61S2/16届蓝桥杯国赛真题'

for i, page in enumerate(doc):
    pix = page.get_pixmap(dpi=200)
    img = Image.frombytes('RGB', [pix.width, pix.height], pix.samples)
    text = pytesseract.image_to_string(img, lang='chi_sim+eng')
    outpath = os.path.join(outdir, f'ocr_page_{i+1:02d}.txt')
    with open(outpath, 'w', encoding='utf-8') as f:
        f.write(text)
    print(f'Page {i+1}: {len(text)} chars saved.')

doc.close()
print('All done!')
