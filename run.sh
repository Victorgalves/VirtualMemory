#!/bin/bash

# Função para executar testes
run_tests() {
  command=$1
  test_number=$2
  echo "---------------------------------"
  echo "Executando testes para $command"
  echo "---------------------------------"

  # Loop through the input files

  make clean
  make

  # Definir o intervalo de testes com base no argumento test_number
  if [ "$test_number" = "all" ]
  then
    range="{1..6}"
  else
    range="$test_number"
  fi

  for i in $(eval echo $range)
  do
    # Definir addresses como o caminho para o arquivo de entrada
    addresses="./test/$i/addresses_$i.txt"

    comparefile="./test/${i}/addresses_${i}_${command}.txt"

    # Executar o comando no arquivo de entrada
    ./vm $addresses $command

    # Comparar a saída com o arquivo comparefile
    total_lines=$(wc -l < "$comparefile")
    diff_lines=$(diff -y --suppress-common-lines correct.txt "$comparefile" | wc -l)
    same_lines=$((total_lines - diff_lines))
    
    # Calcular a porcentagem de similaridade
    if [ $total_lines -gt 0 ]
    then
      similarity=$((100 * same_lines / total_lines))
    else
      similarity=0
    fi

    if diff -a correct.txt "$comparefile" > /dev/null
    then
      echo "Teste $addresses: Passou"
      echo "---------------------------------"
    else
      echo "Teste $addresses: Não passou"
      echo
      # Criar o diretório diffs se ele não existir
      mkdir -p diffs
      # Definir o nome do arquivo diff
      diff_file="diffs/diff_${comparefile##*/}"
      # Escrever as diferenças no arquivo diff
      diff -a correct.txt "$comparefile" > "$diff_file"
      # Informar que as diferenças foram escritas no arquivo diff
      echo "As diferenças foram escritas em $diff_file"
      echo
      echo "Similaridade: $similarity%"
      echo "---------------------------------"
    fi
  done
}

# Verificar se um argumento foi fornecido
if [ $# -eq 0 ]
then
  echo "Por favor, forneça um argumento. Use 'all' para executar todos os testes ou um número para executar um teste específico."
  exit 1
fi

# Executar testes para fifo e lru sequencialmente
run_tests fifo $1
run_tests lru $1

# Executar make clean ao final do script
make clean

# Mensagem de créditos
echo "---------------------------------"
echo " Créditos para os contribuidores "
echo " Testes criados e validados por: "
echo "---------------------------------"
echo "Eurivaldo Filho"
echo "Claudio Alves"
echo "Guilherme Cardoso"
echo "Pedro Lira"
echo "Igor Wanderley"
echo "Felipe Serpa"
echo "Gabriel Lima"
echo "---------------------------------"