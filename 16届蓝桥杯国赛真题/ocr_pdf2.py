import fitz, os, pytesseract
from PIL import Image, ImageFilter, ImageEnhance

pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'
doc = fitz.open(r'c:/Users/WYC/Desktop/My code/STC15F2K61S2/16届蓝桥杯国赛真题/16届国赛(已优化).pdf')
outdir = r'c:/Users/WYC/Desktop/My code/STC15F2K61S2/16届蓝桥杯国赛真题'

for i, page in enumerate(doc):
    # Higher DPI for better OCR
    pix = page.get_pixmap(dpi=300)
    img = Image.frombytes('RGB', [pix.width, pix.height], pix.samples)

    # Enhance contrast for better OCR
    enhancer = ImageEnhance.Contrast(img)
    img = enhancer.enhance(2.0)
    enhancer = ImageEnhance.Sharpness(img)
    img = enhancer.enhance(2.0)

    # OCR with better config - try both combined and separate
    text = pytesseract.image_to_string(img, lang='chi_sim+eng', config='--psm 6')

    outpath = os.path.join(outdir, f'ocr2_page_{i+1:02d}.txt')
    with open(outpath, 'w', encoding='utf-8') as f:
        f.write(text)
    print(f'Page {i+1}: {len(text)} chars saved.')
    print(text[:200])

doc.close()
print('All done!')
