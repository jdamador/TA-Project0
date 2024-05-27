# TA-Project0
In this project, the user would describe a Finite State Automaton and then give it a series of rows that the automaton would evaluate and indicate whether to accept or reject them. All programming must be done in C language on Linux, using GTK and Glade.


## Prerrequisitos: instalación del ambiente.
1. Installar las herramientas necesarias para correr el programa en un nuevo ambiente. 
Nota: glace es solo para visualizar el `.glade` que se encuentra en la carpeta `/src`.
```
sudo apt install glace evince gcc libgtk-3-dev
```
2. Instalar el compilador de archivos `.text` para generar PDFs con latex.
```
sudo apt-get install texlive-latex-base
sudo apt-get install texlive-fonts-recommended
sudo apt-get install texlive-fonts-extra
```
## Ejecutar el programa una vez configurado el ambiente.
Una vez que cumple con los pre-requisitos, puede compilar el programa con el siguiente comando:
1. Compilar el código `c` para generar un ejecutable:
```
$ make
```

Ejecutar el programa:
```
$ ./App/run
```

## Información importante sobre la ejecución:
1. El primer paso para utilizar la aplicación consiste en generar la tabla de configuraciones para el DFA deseado representado como una matrix de `tamaño n_states x m_alphabet_symbols`.
2. Una vez establecidos los parametros del DFA, se puede proceder a llenar las celdas de de la tabla de transicioens y seleccionar los estados de aceptación; para ello se puede utilizar los checkbox en el margen izquierdo de la interfaz. Los nombres de los estados estan por defecto y a nivel lógico establecidos de 1 a n_states, pero se pueden establecer nombres manualmente usando el espacio disponible, estos nombres solo se veran reflejados a nivel de salida, la configuación de la tabla de transiciones debe realizarse utilizando los nombres por defecto.
3. Una vez que ha finalizado la configuración del DFA, debe indicarlo presionando el botón "Evaluate" que se habilite la parte superior derecha de la GUI, donde se puede imprimir la información del DFA o evaluar directamente una hilera sobre el DFA.
4. El boton de imprimir genera un pdf con la información del DFA, la información generada es la siguiente.
- La definición formal del DFA mostrando los valores para cada uno de los elementos del quinteto.
- El diagrama del DFA configurado en la tabla de transiciones.
- Ejemplos de hileras aceptadas y rechazadas por el DFA. **NOTA**: La generación de estas hileras se realiza de manera aleatoria, por ende, puede demorarse algunos segundos en cargar el PDF resultado, si la interfaz se congela durante el proceso NO CLICKEAR DE MÁS el programa o se producira un error que forzará el cierre del visualidor externo (EVINCE).
5. El último punto visualizado es la expresión regular con el teorema de Arden. Este punto se encuentra incompleto, pero se genera la mitad del algoritmo.