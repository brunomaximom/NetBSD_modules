# echo
Um módulo que simula o comportamento do comando echo em módo kernel.
Passa o argumento através de uma dicionário property list com o parâmetro -s seguido de chave=valor. 
A chave usada é "str" e o valor é qualquer string. Se a string do valor começar com -n, então não pulará linha ;)
Repare também no uso da macro __UNCONST