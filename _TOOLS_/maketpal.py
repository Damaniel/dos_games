rows = [[0, 0, 0], [0, 0, 170], [0, 170, 0], [0,170, 170],
        [170, 0, 0], [170, 0, 170], [170, 85, 0], [170, 170, 170]]
cols =  [[0, 0, 0], [0, 0, 170], [0, 170, 0], [0,170, 170],
        [170, 0, 0], [170, 0, 170], [170, 85, 0], [170, 170, 170],
        [85, 85, 85], [85, 85, 255], [85, 255, 85], [85, 255, 255],
        [255, 85, 85], [255, 85, 255], [255, 255, 85], [255, 255, 255]]

print("GIMP Palette")
print("Name: Text mode dither")
print("Columns: 16")
print("#")

#for i in range(len(rows)):
#    for j in range(len(cols)):
#        # 25% mix
#        r = int((rows[i][0] + cols[j][0]*3) / 4)
#        g = int((rows[i][1] + cols[j][1]*3) / 4)
#        b = int((rows[i][2] + cols[j][2]*3) / 4)       
#        print(f"{r: 4} {g: 4} {b: 4}")       

for i in range(len(rows)):
    for j in range(len(cols)):  
        # 50% mix
        r = int((rows[i][0] + cols[j][0]) / 2)
        g = int((rows[i][1] + cols[j][1]) / 2)
        b = int((rows[i][2] + cols[j][2]) / 2)        
        print(f"{r: 4} {g: 4} {b: 4}")

#for i in range(len(rows)):
#    for j in range(len(cols)):        
#        # 75% mix
#        r = int((rows[i][0]*3 + cols[j][0]) / 4)
#        g = int((rows[i][1]*3 + cols[j][1]) / 4)
#        b = int((rows[i][2]*3 + cols[j][2]) / 4)
#        print(f"{r: 4} {g: 4} {b: 4}")   

for i in range(len(rows)):
    for j in range(len(cols)):        
        # 100% mix
        r = int(cols[j][0])
        g = int(cols[j][1])
        b = int(cols[j][2])
        print(f"{r: 4} {g: 4} {b: 4}")                

