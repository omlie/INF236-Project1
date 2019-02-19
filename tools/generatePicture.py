from PIL import Image

rows = []
with open('allIterations.txt') as fp:
    for row in fp:
        r = list(map(int, list(row.split()[0])))
        rows.append(r)

(r, c) = (len(rows), len(rows[0]))

img = Image.new('1', (c, r))
pixels = img.load()

for x in range(r):
    for y in range(c):
        pixels[y, x] = not rows[x][y]


img.save('image.bmp')