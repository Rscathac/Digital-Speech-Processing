import sys

read_file = sys.argv[1]
write_file = sys.argv[2]

# Some Chinese character can't be decode by big-5, so use big5-HKSCS
file_object_in  = open(read_file, 'r', encoding = 'BIG5-HKSCS')
file_object_out = open(write_file, 'w', encoding = 'BIG5-HKSCS')

# Create a dynamic dict to link keys(ZhuYin) & values(characters)
Map = {}

for line in file_object_in:
    x = line.split(' ')
    character = x[0]
    ZhuYin = x[1].split('/')
#    print(character)

    for i in ZhuYin:
        if i[0] in Map:
            Map[i[0]].append(character)
        else:
            Map[i[0]] = [character]

    Map[character] = character


for Zhuyin in sorted(Map.keys()):
    file_object_out.write(Zhuyin + '   ')
    
    for word in Map[Zhuyin]:
        file_object_out.write(' '+word)
    file_object_out.write('\n')

file_object_out.close()
file_object_in.close()
