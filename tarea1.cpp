/*
 * Suma Paralela de Arreglos usando OpenMP
 * Programa que suma dos arreglos utilizando paralelizacion con OpenMP
 *
 * Autor: Luis Daniel Ruvalcaba Hernandez
 * Fecha: 01/Feb/2026
 */

#include <iostream>
#include <omp.h>
#include <cstdlib>
#include <ctime>
#include <iomanip>

using namespace std;

// FunciC3n para imprimir un rango de elementos de un arreglo
void imprimirArreglo(const string& nombre, int arreglo[], int total, int mostrar = 20) {
	cout << "\n" << nombre << " (mostrando ";
	if (mostrar >= total) {
		cout << "todos los " << total << " elementos";
	} else {
		cout << "primeros " << mostrar << " de " << total << " elementos";
	}
	cout << "):" << endl;

	int limite = (mostrar < total) ? mostrar : total;
	for (int i = 0; i < limite; i++) {
		cout  << arreglo[i] << "  ";
		if ((i + 1) % 10 == 0) cout << endl;
	}
	if (limite % 10 != 0) cout << endl;
}

int main() {
	int n;  // Tamaño de los arreglos
	int opcion;
	int numHilos;

	cout << "============================================================" << endl;
	cout << "         SUMA PARALELA DE DOS ARREGLOS CON OpenMP" << endl;
	cout << "============================================================" << endl;

	// Obteniendo la cantidad de elementos en numero par
	do {
		cout << "\nIngrese el tamaño de los arreglos: ";
		cin >> n;

		if (n <= 0) {
			cout << "Error: La cantidad debe ser mayor a 0." << endl;
		}
	} while (n <= 0 );


	// Preguntar nC:mero de hilos
	do {
		cout << "\nIngrese el numero de hilos a utilizar: ";
		cin >> numHilos;

		if (numHilos <= 0) {
			cout << "Error: El numero de hilos debe ser mayor a 0." << endl;
		}
	} while (numHilos <= 0);

	omp_set_num_threads(numHilos);

	// Creando arreglos y asignandoles valores, dando la opcion a aleatorios o secuenciales
	int* arregloA = new int[n];
	int* arregloB = new int[n];
	int* arregloR = new int[n];

	cout << "\n" << endl;
	cout << "Indique como desea llenar los arreglos:" << endl;
	cout << "1. Generar valores secuenciales" << endl;
	cout << "2. Valores aleatorios (0-99)" << endl;
	cout << "Opcion: ";
	cin >> opcion;

	if (opcion == 1) {
		// Opcion 1: Valores secuenciales
		for (int i = 0; i < n; i++) {
			arregloA[i] = i + 1;
			arregloB[i] = i + 1 + n;
		}
	} else {
		// OpciC3n 2: Valores aleatorios
		srand(time(NULL));
		for (int i = 0; i < n; i++) {
			arregloA[i] = rand() % 100;
			arregloB[i] = rand() % 100;
		}
	}

	// Mostrar los arreglos originales
	cout << "\n----------------------- Arreglos originales -----------------------" << endl;

	int elementosMostrar = (n <= 50) ? n : 20;
	imprimirArreglo("Arreglo A", arregloA, n, elementosMostrar);
	imprimirArreglo("Arreglo B", arregloB, n, elementosMostrar);

	// 3. REALIZAR FOR PARALELO CON OpenMP

	cout << "\n---------------- Ejecutando la suma paralela --------------------" << endl;
	cout << "\nNumero de hilos configurados: " << numHilos << endl;

	// Registrar tiempo de inicio
	double inicio = omp_get_wtime();

	// Region paralela
	#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int totalHilos = omp_get_num_threads();

		// Mostrar informacion de hilos (solo una vez)
		#pragma omp single
		{
			cout << "Hilos ejecutandose: " << totalHilos << endl;
			cout << "\nProcesando suma paralela..." << endl;
		}

		// FOR paralelo - OpenMP distribuye automaticamente las iteraciones
		#pragma omp for schedule(static)
		for (int i = 0; i < n; i++) {
			arregloR[i] = arregloA[i] + arregloB[i];

			// Mostrando informacion detallada solo para primeros elementos
			if (i < 20) {
				#pragma omp critical
				{
					cout << "  Hilo " << id << ": R[" << i << "] = "
					     << arregloA[i] << " + "
					     << arregloB[i] << " = "
					     << arregloR[i] << endl;
				}
			}
		}

		// Sincronizar antes de continuar
		#pragma omp barrier
	}

	// Registrar tiempo final
	double fin = omp_get_wtime();

	cout << "\nSuma completada exitosamente!" << endl;
	cout << "Tiempo de ejecucion: " << (fin - inicio) * 1000 << " ms" << endl;

	// Imprimir el resultado de la suma para comprobar los resultados
	cout << "\n------------------------- Arreglo final -------------------------" << endl;


	imprimirArreglo("Arreglo Resultado (A + B)", arregloR, n, elementosMostrar);

	// Verificacion de resultados

	cout << "\n------------------ Verificacion de resultado --------------------" << endl;

	int elementosVerificar = (n <= 10) ? n : 10;
	cout << "\nVerificando los primeros " << elementosVerificar << " elementos:" << endl << endl;

	bool todoCorrecto = true;
	for (int i = 0; i < elementosVerificar; i++) {
		int esperado = arregloA[i] + arregloB[i];
		cout << "Posicion " << setw(4) << i << ": "
		     << setw(3) << arregloA[i] << " + "
		     << setw(3) << arregloB[i] << " = "
		     << setw(4) << arregloR[i];

		if (arregloR[i] == esperado) {
			cout << "  -  Correcto" << endl;
		} else {
			cout << "  -  Error (esperado: " << esperado << ")" << endl;
			todoCorrecto = false;
		}
	}

	// En el siguiente codigo se verifican todos los elementos (sin imprimir)
	if (todoCorrecto && n > elementosVerificar) {
		cout << "\nVerificando elementos restantes..." << endl;
		for (int i = elementosVerificar; i < n; i++) {
			if (arregloR[i] != arregloA[i] + arregloB[i]) {
				todoCorrecto = false;
				cout << "Error encontrado en posicion " << i << endl;
				break;
			}
		}
	}

	if (todoCorrecto) {
		cout << "RESULTADO: Todos los resultados son CORRECTOS!" << endl;
	} else {
		cout << "RESULTADO: Se encontraron ERRORES en los resultados." << endl;
	}


	// Estadisticas finales
	cout << "\n------------------------- ESTADISTICAS -------------------------" << endl;
	cout << "Total de elementos procesados: " << n << endl;
	cout << "Numero de hilos utilizados: " << numHilos << endl;
	cout << "Elementos por hilo (aprox): " << n / numHilos << endl;
	cout << "Tiempo de ejecucion: " << (fin - inicio) * 1000 << " ms" << endl;

	// Liberar memoria
	delete[] arregloA;
	delete[] arregloB;
	delete[] arregloR;

	cout << "\nPrograma finalizado correctamente." << endl;
	cout << "============================================================" << endl;

	return 0;
}
