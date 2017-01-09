# evitar problemas con ddd 
# elimina archivos que se han generado automaticamente.
# realizar cada vez que se quede en modo de espera.
# cerrar el ddd y ejecutar el script.
# uso: ./eliminar_ddd.sh 

# cambia de directorio donde se encuentran los archivos a eliminar
cd ~/.ddd
# elimina los 3 archivos que se han generado
rm history log init

