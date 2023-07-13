cmd_/home/orochy/Practica/day2/dev2.mod := printf '%s\n'   dev2.o | awk '!x[$$0]++ { print("/home/orochy/Practica/day2/"$$0) }' > /home/orochy/Practica/day2/dev2.mod
