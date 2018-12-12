import string

up = 8
down = 7
down_counter = 0
up_counter = 0
filter_width = 2*down + 1
rows = filter_width + up * down
m = [['.']*filter_width for x in range(0, rows)]
x = ['x']*rows;
y = ['x']*rows;


for k in range(0, rows, up):
    x[k] = str(up_counter)
    up_counter += 1

for k in range(0, rows, down):
    y[k] = 'y' + str(down_counter)
    down_counter += 1

for k in range(0,filter_width):
    up_counter = 0;
    for j in range(k, rows):
        m[j][k] = x[up_counter]
        up_counter += 1

label = [string.ascii_lowercase[x] for x in range(0,filter_width)]
print(label)
for i in range(0, rows, down):
    print(str(m[i]) + " " + y[i])
