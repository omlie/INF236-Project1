from PIL import Image

rows = []
with open('allIterations.txt') as fp:
    for row in fp:
        rows.append(row.rstrip('\n'))

(r, c) = (len(rows), len(rows[0]))

img = Image.new('1', (r, c))
pixels = img.load()

for y in range(r):
    for x in range(c):
        pixels[y, x] = 1 if rows[y][x] == '0' else 0


img.save('image.bmp')