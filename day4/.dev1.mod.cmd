cmd_/media/sf_/Practica/day4/dev1.mod := printf '%s\n'   dev1.o | awk '!x[$$0]++ { print("/media/sf_/Practica/day4/"$$0) }' > /media/sf_/Practica/day4/dev1.mod
