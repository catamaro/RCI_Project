# Tabela	de	Chaves Distribuída (RCI_Project)
  Uma tabela de chaves distribuída é uma versão simplificada de uma tabela de hash 
distribuída. Uma tabela de hash é uma estrutura de dados constituída por pares (chave,
valor), em que chave é um número inteiro e valor referencia um objeto associado à chave.
Cada valor é pesquisado pela sua chave. Por exemplo, as chaves podem ser números de
cartões de cidadão e os valores podem ser registos com o nome, data de nascimento, sexo
e morada dos portadores dos cartões.

  Geralmente, o espaço de chaves é demasiado grande para uma pesquisa eficiente.
Recorre-se então a uma função de hash que mapeia chaves arbitrárias em outras chaves
pertencentes a um conjunto pequeno de números inteiros.

  Numa tabela de hash distribuída, os pares (chave, valor) são repartidos por vários
servidores por forma a que a pesquisa de uma chave seja eficiente e a entrada e a saída de
um servidor não altere os pares (chave, valor) associados à maioria dos servidores. As
tabelas de hash distribuídas são usadas, entre outros, em redes de distribuição de
conteúdos.

  Uma tabela de chaves distribuída compreende as funções de pesquisa de uma chave,
entrada e saída de servidores, mas ao contrário de uma tabela de hash distribuída
propriamente dita, dispensa a recuperação, atualização e migração entre servidores dos
valores associados às chaves. 

## Interface de Utilizador

A interface de utilizador consiste nos seguintes comandos:

• new i - Criação de um novo anel composto exclusivamente pelo servidor i.

• entry i boot boot.IP boot.TCP - Entrada do servidor i no anel ao qual pertence o servidor boot com endereço IP
boot.IP e porto boot.TCP.

• sentry i succi succi.IP succi.TCP - Entrada do servidor i no anel no qual o servidor succi com endereço IP
succi.IP e porto succi.TCP será o sucessor do servidor i. Este comando
pressupõe que se sabe que a chave i não é usada por um servidor pertencente ao
anel e que succi será o sucessor do servidor i. Ele serve para construir um anel
um servidor à vez sem necessitar da pesquisa de chaves.

• leave - Saída do servidor do anel.

• show - Mostra do estado do servidor, incluindo a sua chave, endereço IP e porto, bem
como os valores correspondentes do seu sucessor e segundo sucessor.

• find k - Pesquisa do servidor que armazena a chave k, com a apresentação da sua chave,
endereço IP e porto.

• exit - Fecho da aplicação.
