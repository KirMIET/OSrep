#!/bin/bash

echo "Путь"
pwd
echo ""

echo "Дата"
dt=$(date '+%d/%m/%Y %H:%M:%S');
echo "$dt"
echo ""

echo "Переменные окружения"
echo "$PATH" | tr ':' '\n'