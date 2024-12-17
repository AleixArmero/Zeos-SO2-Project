
file = open("mapa.csv")

write_file = open('mapa.txt', "w")

write_file.write("char map[25][80][2] = {\n")

for i in range(25):
    line = file.readline().split(',')
    write_file.write("{")
    for j in range(80):
        if int(line[j]) == 16:
            write_file.write("{'#', 0x0F},")
        elif int(line[j]) == 89:
            write_file.write("{'[', 0x0A},")
        else:
            write_file.write("{' ', 0x00},")
    write_file.write("},\n")

write_file.write("};")
