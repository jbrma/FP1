// MIGUEL AGUILERA Y JORGE BRAVO GRUPO E

#include    <iostream>
#include    <iomanip>
#include    <cctype>
#include	<string>	
#include	<cstdlib>
#include    <fstream>
#include	<ctime>	
#include	<limits>	//	Para	pausa()	
#ifdef	_WIN32	//	Se	define	automáticamente	en	Visual	Studio
#include	<windows.h>	
#undef	max //	Para	poder	usar	max()	en	pausa()

#endif	

using namespace std;

typedef enum { Amarillo, Azul, Rojo, Verde, Gris, Ninguno }	tColor;

const int	NUM_JUGADORES = 4;
const int	NUM_FICHAS = 4;
const int	NUM_CASILLAS = 68;
const int   META = 108;

typedef struct tCasilla {

    tColor calle1;
    tColor calle2;
};
typedef tCasilla tCasillas[NUM_CASILLAS];
typedef int tFichas[NUM_FICHAS];


typedef struct tJugador {

    tColor color;
    tFichas fichas;
};
typedef tJugador tJugadores[NUM_JUGADORES];


typedef struct tJuego {

    tCasillas casillas;
    tColor jugadorTurno;
    tJugadores jugadores;

    int ultimaFichaMovida;
    int seises;
    int premio;
    int tirada;

};

const string Archivo = "pruebas5.txt";
const bool	Debug = true;

//Funciones implementadas 
bool esSeguro(int casilla);
int salidaJugador(tColor jugador);
int zanataJugador(tColor jugador);
string colorACadena(tColor	color);
void mostrar(const tJuego& juego);
void cargar(tJuego& juego);

void setColor(tColor color);
void pausa();
void iniciaColores();
void iniciar(tJuego& juego);
bool puente(const tJuego& juego, int casilla);
int cuantasEn(const tFichas jugador, int casilla);
int primeraEn(const tFichas jugador, int casilla);
int segundaEn(const tFichas jugador, int casilla);
void saleFicha(tJuego& juego);
void aCasita(tJuego& juego, int casillas);
bool todasEnMeta(const tFichas jugador);

// Funciones para la dinamica del juego

void abrirPuente(tJuego& juego, int casilla, int casilla2);
bool procesa5(tJuego& juego, bool& pasaTurno);
bool procesa6(tJuego& juego, bool& pasaTurno);
bool jugar(tJuego& juego, bool& fin);
bool puedeMover(tJuego& juego, int ficha, int& casilla);
void mover(tJuego& juego, int ficha, int casilla);
int lanzardados();



int main()
{
    tJuego juego;
    tJugadores jugadores;
    tCasillas calle1, calle2;

    bool fin = false;
    juego.seises = 0; //Contador de seises

    iniciaColores();
    iniciar(juego);

    if (Debug)
        cargar(juego);
    mostrar(juego);

    while (!fin) {

        int salida = 0;
        bool pasaTurno = true;
        bool jugada = false;

        setColor(juego.jugadorTurno);

        if (juego.premio > 0) //Si el jugador ha ganado un premio
        {
            cout << "Has ganado un premio! Cuenta " << juego.premio << " casillas. " << endl;
            juego.tirada = juego.premio;
            juego.premio = 0;
        }
        else
        {
            cout << "Turno del jugador de color " << colorACadena(juego.jugadorTurno) << endl;

            if (Debug) {

                cout << "Introduce tu tirada ( 0 si quieres salir): ";
                cin >> juego.tirada;
                cin.get();

                if (juego.tirada == 0) {

                    return 0;
                }
            }
            else {

                juego.tirada = lanzardados();
            }
        }

        pasaTurno = true;

        // Para finalizar el juego la tirada debe ser 0

        if (juego.tirada == 0)
        {
            fin = true;
            jugada = true;
        }

        else if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) == NUM_FICHAS && juego.tirada != 5)  //Si todas las fichas están en casa y sale una tirada distinta a 5, no se podrá salir
        {
            cout << "No puedes mover, tienes todas las fichas en casa..." << endl;

            jugada = true; // Evita que se ejecute jugar() e imprime las diferentes opciones de movimiento

            if (juego.tirada == 6 && juego.seises < 2) {

                pasaTurno = false;
                juego.seises++;

            }              
        }
        else if (juego.tirada == 5)
        {
            jugada = procesa5(juego, pasaTurno);

            if (jugada)
            {
                cout << "Una ficha sale de casa..." << endl;
            }
            else
            {
                cout << "No es posible sacar una ficha de casa..." << endl;
               
            }            
        }
        else if (juego.tirada == 6)
        {
            jugada = procesa6(juego, pasaTurno);

        }

        if ((juego.tirada > 7) && (juego.seises > 0))	// Premio tras un 6	
            pasaTurno = false;

        if (!jugada)
        {
            pasaTurno = jugar(juego, fin);
        }

        // En caso de pasar turno de jugador por el de otro color, se reinician los seises
        if (pasaTurno && !fin)
        {
            juego.jugadorTurno = tColor((int(juego.jugadorTurno) + 1) % NUM_JUGADORES);

            // El contador de seises a cero

            juego.seises = 0;           
        }

        pausa();
        mostrar(juego);

        fin = todasEnMeta(juego.jugadores[juego.jugadorTurno].fichas);

    }

    if (fin) {

        cout << "HA GANADO EL JUGADOR DE COLOR " << colorACadena(juego.jugadorTurno) << " !!! " << endl;
    }

    return 0;
}



// FUNCIONES IMPLEMENTADAS

// Funcion que devuelve las casillas seguras 

bool esSeguro(int pos)
{
    bool seg = false;


    if ((pos == 0) || (pos == 12) || (pos == 17) || (pos == 29) || (pos == 34) || (pos == 46) || (pos == 51) || (pos == 63) || (pos == 5) || (pos == 22) || (pos == 39) || (pos == 56))
    {
        seg = true; //Si se encuentra en una de las casillas seguras devuelve true
    }

    return seg;
}

// Devuelve la casilla de salida de cada jugador 
int salidaJugador(tColor jugador)
{
    int inicio = 0;

    if (jugador == Amarillo)
        inicio = 5;

    else if (jugador == Azul)
        inicio = 22;

    else if (jugador == Rojo)
        inicio = 39;

    else if (jugador == Verde)
        inicio = 56;


    return inicio;
}

// Funcion que indica la zanata de cada jugador que son 5 casillas menos de la casilla de inicio
int zanataJugador(tColor jugador)
{
    int zanata = 0;

    if (jugador == Amarillo)
        zanata = 0;

    else if (jugador == Azul)
        zanata = 17;

    else if (jugador == Rojo)
        zanata = 34;

    else if (jugador == Verde)
        zanata = 51;

    return zanata;
}

// Funcion que pasa el color que corresponde a cadena
string colorACadena(tColor	color)
{
    string nomColor;

    if (color == Amarillo)
    {
        nomColor = "Amarillo";
    }
    else if (color == Azul)
    {
        nomColor = "Azul";
    }
    else if (color == Rojo)
    {
        nomColor = "Rojo ";
    }
    else if (color == Verde)
    {
        nomColor = "Verde ";
    }
    else if (color == Ninguno)
    {
        nomColor = "Ninguno";

    }
    else if (color == Gris)
    {
        nomColor = "Gris";
    }
    return nomColor;
}



void setColor(tColor color) {
    switch (color) {
    case Azul:
        cout << "\x1b[34;107m";
        break;
    case Verde:
        cout << "\x1b[32;107m";
        break;
    case Rojo:
        cout << "\x1b[31;107m";
        break;
    case Amarillo:
        cout << "\x1b[33;107m";
        break;
    case Gris:
    case Ninguno:
        cout << "\x1b[90;107m";
        break;
    }
}
void pausa() {
    cout << "Pulsa Intro para continuar...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}
void iniciaColores() {
#ifdef _WIN32
    for (DWORD stream : {STD_OUTPUT_HANDLE, STD_ERROR_HANDLE}) {
        DWORD mode;
        HANDLE handle = GetStdHandle(stream);

        if (GetConsoleMode(handle, &mode)) {
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(handle, mode);
        }
    }
#endif
}

// Inicializa todas las casillas y colores y asigna un numero aleatorio para el turno del jugador que empieza primero
void iniciar(tJuego& juego)
{

    setColor(Gris);
    srand(time(NULL));
    tColor primerJugador;

    // Numero aleatorio de 0 a 3 para saber el jugador que empieza
    primerJugador = tColor(0 + rand() % (4));

    juego.jugadorTurno = primerJugador;

    // Se inicializan todas las fichas en posicion -1, es decir, en casa
    for (int j = 0; j < 4; j++) {
        for (int f = 0; f < 4; f++)
            juego.jugadores[j].fichas[f] = -1;
    }

    // Inicializa la calle 1 en color ninguno
    for (int c = 0; c < NUM_CASILLAS; c++)
    {
        juego.casillas[c].calle1 = Ninguno;
        juego.casillas[c].calle2 = Ninguno;
    }


}

// Funcion que devuelve true o false si hay un puente o no 
bool puente(const tJuego& juego, int casilla)
{
    bool hayPuente = false;

    //Comprobamos que la casilla de las 2 calles sea igual, y que la calle 1 está ocupada para así formar el puente
    if ((juego.casillas[casilla].calle1 == juego.casillas[casilla].calle2) && (juego.casillas[casilla].calle1 != Ninguno) && casilla > -1 && casilla < NUM_CASILLAS)
    {
        hayPuente = true;
    }
    
    return hayPuente;
}

// Funcion que devuelve cuantas fichas hay en una casilla
int cuantasEn(const tFichas jugador, int casilla)
{
    int totalFichas = 0;
    for (int i = 0; i < NUM_FICHAS; i++)
    {
        if (jugador[i] == casilla)
        {
            totalFichas++;
        }
    }

    return totalFichas;
}

// Devuelve el menor indice de la fichas que se encuentra  en esa casilla
int primeraEn(const tFichas jugador, int casilla)
{

    bool enc = false;
    int ind = 0;

    while ((ind < 4) && (!enc))
    {
        if (jugador[ind] == casilla)
        {
            enc = true;
        }
        else
        {
            ind++;
        }
    }
    if (!enc) {

        ind = -1;
    }
    return ind;
}

// Devuelve el segundo menor indice de las fichas que se encuentran en una casilla
int segundaEn(const tFichas jugador, int casilla)
{

    bool segunda = false;
    int ind = NUM_FICHAS;

    while ((ind >= 0) && (!segunda))
    {
        if (jugador[ind] == casilla)
        {
            segunda = true;

        }
        else
        {
            ind--;
        }
    }
    if (!segunda) {

        ind = -1;
    }
    return ind;

}

// Sale	de casa una ficha del jugador, concretamente la	de menor índice	que	esté en	casa. La que sale va a la calle 1 y si hubiera otra se coloca en la calle 2
void saleFicha(tJuego& juego)
{
    int saleFicha = 0;
    int sal = salidaJugador(juego.jugadorTurno);

    saleFicha = primeraEn(juego.jugadores[juego.jugadorTurno].fichas, -1); //Sale la de menor indice
    juego.jugadores[juego.jugadorTurno].fichas[saleFicha] = sal;
    juego.casillas[sal].calle2 = juego.casillas[sal].calle1;
    juego.casillas[sal].calle1 = juego.jugadorTurno;

}

// Manda a casa la ficha que se encuentra en la casilla
void aCasita(tJuego& juego, int casilla) {

    tColor col;
    int fichaEn; // Si hay una o dos fichas en la misma casilla de calle 1 y calle 2

    col = juego.casillas[casilla].calle2;
    fichaEn = segundaEn(juego.jugadores[col].fichas, casilla);
    juego.jugadores[col].fichas[fichaEn] = -1; // Manda a casa la ficha del otro jugador
    juego.casillas[casilla].calle2 = Ninguno;

}

// Comprueba si todas las fichas están en meta
bool todasEnMeta(const tFichas jugador) {

    bool fin = false;
    int meta = 108;

    //Bucle que recorra todas las fichas del mismo jugador para saber si estan en la casilla final (108)
    for (int f = 0; f < NUM_FICHAS; f++) {
        if (jugador[f] == meta)
            fin = true;
        else
            fin = false;
    }

    return fin;
}



// FUNCIONES PARA LA DINAMICA DEL JUEGO

// Cuando se llama a esta función, abre el puente
void abrirPuente(tJuego& juego, int casilla, int casilla2) {

    int mueveFicha2 = segundaEn(juego.jugadores[juego.jugadorTurno].fichas, casilla); // Se mueve la ficha de la calle 2
    cout << endl << "Se ha abierto un puente " << endl;
    mover(juego, mueveFicha2, casilla2);
    juego.ultimaFichaMovida = mueveFicha2;

}

// Intenta sacar una ficha de casa del jugador, si queda alguna.
bool procesa5(tJuego& juego, bool& pasaTurno) {

    bool sacaFicha = false;
    int salida = 0; // Es la ficha que está en la calle;
    salida = salidaJugador(juego.jugadorTurno);


    //Comprueba que hay al menos una ficha en casa y que al menos una de las dos calles no esté ocupada por el jugador

    if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) != 0 ) {
        //CuantasEn indica cuantas fichas hay en una casilla, el -1 indica la casa, por tanto queremos saber que hay al menos 1 en casa


        if (juego.casillas[salida].calle2 != Ninguno) { // Si existe una ficha en la calle 2, se dan dos casos, que sea tuya o no

            if (juego.casillas[salida].calle2 == juego.jugadorTurno) { // Si es tuya, no sacas mas fichas

                sacaFicha = false;
            }
            else if (juego.casillas[salida].calle2 != juego.jugadorTurno) { // Si no es tuya, te la comes y cuentas 20

                cout << "Te comiste una ficha de otro jugador!" << endl;
                aCasita(juego, salida);
                juego.casillas[salida].calle2 = Ninguno; //La calle 2 quedaría vacía
                juego.premio = 20;
                saleFicha(juego);
                pasaTurno = false; //No pasa de turno ya que ha ganado un premio de 20 al comerse una ficha
                sacaFicha = true;
            }
        }
        else if (juego.casillas[salida].calle1 == Ninguno || juego.casillas[salida].calle2 == Ninguno) { // Si no hay fichas en la calle 1, puede sacar ficha

            saleFicha(juego);
            sacaFicha = true;

        }  
    }

    return sacaFicha;
}

// Procesa	una	tirada	de	6	para	el	jugadorTurno
bool procesa6(tJuego& juego, bool& pasaTurno) {

    bool proc6 = false;


    int contPuentes = 0, ficha2, contFichas = 0, fcasilla;
    const int jugador = 0;
   // int casilla = 0; // Es la ficha que está en la calle

   // casilla = juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida];


    juego.seises++; // Se suma un seis cada vez que se llama a esta función


    if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) == 0) { // Si no tiene fichas en casa

        juego.tirada = 7; //La tirada se convierte en 7 si no tiene fichas en casa
        cout << endl << "Al no tener fichas en casa, cuentas 7!" << endl;
    }

    if (juego.seises == 3) { //Si el contador de seises es 3

        cout << endl << "Has sacado tres seises seguidos!! " << endl;

        if (juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida] < 68) { //Comprobamos que la ultima ficha movida no esté en la subida a meta, en caso contrario no volvería a casa
         
            if (cuantasEn(juego.jugadores[juego.jugadorTurno].fichas, -1) != 4 && juego.ultimaFichaMovida != -1) { //Si hay al menos un espacio en casa y la ultima ficha movida no esta en casa

                juego.seises = 0; //El contador de seises se pone a 0            

                //Comprobamos que la ultima ficha movida sea la de menor indice y si comparte casilla con una de mayor indice
               if (juego.ultimaFichaMovida != segundaEn(juego.jugadores[juego.jugadorTurno].fichas, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]) && juego.ultimaFichaMovida == primeraEn(juego.jugadores[juego.jugadorTurno].fichas, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida])) {

                  int casilla2 = juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida] + 1; //Casilla2 es un auxiliar para que podamos cambiar la ficha de mayor indice de sitio 
                  juego.jugadores[juego.jugadorTurno].fichas[segundaEn(juego.jugadores[juego.jugadorTurno].fichas, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida])] = casilla2;
                  juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]].calle2 = juego.jugadorTurno;
                  aCasita(juego, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]); //La ultima ficha movida vuelve a casa
                  juego.premio = 20;
                  juego.jugadores[juego.jugadorTurno].fichas[segundaEn(juego.jugadores[juego.jugadorTurno].fichas, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida])] = juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida];
               }
               else {

                 juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]].calle1 = juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]].calle2;
                 juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]].calle2 = juego.jugadorTurno;
                 aCasita(juego, juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]);
                 juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[juego.ultimaFichaMovida]].calle2 = Ninguno; // La calle 2 quedaría vacia
               }

                cout << endl << "La ultima ficha movida vuelve a casa" << endl;
                             
            }
            else {

                cout << endl << "Ninguna ficha vuelve a casa" << endl;
            }

            pasaTurno = true; //Pasa al siguiente turno y procesa6 se pone a true
            proc6 = true;
        }
        else { // Si la ficha está en la subida de meta, se salva y no vuelve a casa

            cout << endl << "La ultima ficha movida no vuelve a casa ya que esta dentro de la zanata" << endl;

        }
    }
    
    else { // Si el contador aún no está a 3

        for (int i = 0; i < NUM_FICHAS; i++) { // Se recorren todas las fichas del jugador

            if (puente(juego, juego.jugadores[juego.jugadorTurno].fichas[i])) {//Se comprueba si hay algun puente, es decir que dos fichas del mismo color esten en la misma casilla
                //O si son de diferente color estén en un seguro

                contPuentes++; // Si hay 1 puente, se añade al contador
                ficha2 = i;
                fcasilla = juego.jugadores[juego.jugadorTurno].fichas[i];

                if (puedeMover(juego, ficha2, fcasilla)) {

                    contFichas++; // Añadimos una ficha en caso de que se pueda mover
                }
            }
        }
        if (contPuentes != 0 && contPuentes < 8) { // Si hay al menos 1 puente, y menos de 8 (más de 8 es imposible, ya que hay 16 fichas)

            if (contFichas == 2) { // Si hay dos fichas que se pueden mover

                abrirPuente(juego, juego.jugadores[juego.jugadorTurno].fichas[ficha2], fcasilla);
                proc6 = true;
            }
            else if (contFichas == 0) { // No se puede mover ninguna ficha, por lo que tampoco se mueve ningún puente

                cout << endl << "No se puede abrir ningun puente" << endl;

            }
        }
        pasaTurno = false;

    }
    if (juego.seises == 1) {//Si saca un 6, vuelve a tirar
        cout << "Vuelve a tirar! " << endl << endl;
        pasaTurno = false;
    }
    else if (juego.seises == 2) {//Se avisa al jugador de que ha tirado dos seises seguidos
        cout << endl << "Cuidado! Has sacado dos seises seguidos... " << endl << endl;
        pasaTurno = false;
    }
    return proc6;
}

// Esta	función	se invocará	cuando no haya salido una ficha de casa con	un 5, no se	haya ido una ficha a casa con un 6, 
//ni haya habido que abrir un puente con un	movimiento obligatorio.
bool jugar(tJuego& juego, bool& fin)
{
    int  casilla = 0, f = 0;
    bool sigTurno, puedeJug = false;
    int fContMov = 0; //Contador de las fichas que se pueden mover 
    int fichaQueSeMueve;

    for (int i = 0; i < NUM_FICHAS; i++) { //Hace un bucle para las 4 fichas y muestra lo que se puede hacer con cada una

        cout << i + 1 << ": ";

        if (puedeMover(juego, i, casilla)) //Si la ficha se puede mover, mostrará a la casilla que puede ir
        {
            cout << "Puede ir a la casilla " << casilla << endl;
            puedeJug = true;
            fichaQueSeMueve = i;

            fContMov++; //Si hay una ficha que se puede mover, el contador se incrementa

        }
        else {

            cout << "No se puede mover " << endl;
        }
    }

    if (fContMov == 0) { //Si el contador es 0 significa que no se puede mover ninguna ficha y se informa al usuario

        cout << "No se puede mover ninguna ficha" << endl;
    }
    else if( fContMov == 1) { //Si solo hay una ficha que se puede mover, se obliga a mover esa

        if (puedeMover(juego, fichaQueSeMueve, casilla)){

            mover(juego, fichaQueSeMueve, casilla);

            if (juego.premio != 0) { //Si ha obtenido un premio, se informa de cuál ha sido

                cout << "Premio obtenido: " << juego.premio << endl;
            }
        }
    }
    else if (fContMov >= 2) { //Si hay 2 o mas fichas que se pueden mover, se pide cuál quiere mover

        cout << "Ficha a mover: ";
        cin >> f;
        cin.get();
        f--; //Se resta 1 ya que las fichas se empiezan a contar desde 0

        if (puedeMover(juego, f, casilla)) //Si la ficha se puede mover, se mueve
        {
            mover(juego, f, casilla);

            if (juego.premio != 0) { //Si ha obtenido un premio, se informa de cuál ha sido

                cout << "Premio obtenido: " << juego.premio << endl;
            }
        }

    }   
    if ((juego.tirada == 7) || (juego.tirada == 6) || (juego.premio != 0)) {

        //Si la tirada es un 7, un 6 o ha obtenido un premio, aun no pasa de turno

        sigTurno = false;

    }
    else {

        sigTurno = true;
    }

    juego.ultimaFichaMovida = f; //La ultima ficha movida se iguala a f

    return sigTurno;
}


// Indica la casilla a la que puede	ir la ficha del	jugador	con	esa	tirada
bool puedeMover(tJuego& juego, int ficha, int& casilla) {

    bool seMueve = true;
    casilla = juego.jugadores[juego.jugadorTurno].fichas[ficha]; //Es la casilla actual
    int i = 1;
    int proxima = casilla; // Se refiere a la próxima casilla

    if (proxima != -1 && proxima != 108) { // Si la siguiente casilla no son ni casa, ni la meta.


        while (seMueve && i <= juego.tirada) { /* i recorre de uno en uno el movimiento de la ficha, por ejemplo si ha salido un 3 (tirada = 3),
          el bucle finalizaría cuando ya se hayan hecho los 3 movimientos, se sabrá porque i será igual a tirada y por ello, no entrará en el bucle */


          // Si	la	casilla	actual	es	la	zanata	del	jugadorTurno, la	siguiente	es	la	101
            if (proxima == zanataJugador(juego.jugadorTurno)) {

                proxima = 101;

            }
            // Si la casilla actual está en	la subida a meta (> 100), basta incrementar casilla para ir a la siguiente
            else if (proxima > 100) {

                proxima++;

            }
            //es decir, la casilla esstá en la calle, se incrementa casilla teniendo en cuenta que la siguiente a la 67 es la 0
            else if (0 <= proxima && proxima < META) {

                proxima = (proxima + 1) % NUM_CASILLAS;

            }

            // SITUACIONES QUE SE PUEDEN DAR AL PASAR A LA CASILLA SIGUIENTE

            if (puente(juego, proxima) && esSeguro(proxima) || (proxima > 108) ) { // Si en la casilla hay un puente seguro no se puede mover,
                //si la casilla es la meta y aún no es el ultimo movimiento, devuelve false               
                seMueve = false;
            }
            
            i++; //Pasa al siguiente movimiento
        }
    }

    else { //Si el destino de la ficha es casa o meta, no se mueve

        seMueve = false;
    }

    if (puente(juego, proxima) && ((proxima)< NUM_CASILLAS)) { //Si hay un puente no se puede mover

        seMueve = false;
    }

    if (i == (juego.tirada + 1) && seMueve == true) { // Se comprueba que es el ultimo movimiento

        casilla = proxima;
        seMueve = true;
    }

    return seMueve;
}

// Ejecuta	el	movimiento	de	la	ficha
void mover(tJuego& juego, int ficha, int casilla) {

    //Esta función solo se llama cuando se ha comprobado puedeMover() y efectivamente se puede mover

    juego.premio = 0; //Empieza el movimiento por tanto el premio se inicializa a 0

    if (juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ficha]].calle1 == juego.jugadorTurno) {
        // Comienza quitando la ficha de su ubicacion actual en calle 1 o calle 2, si hay otra ficha en la casilla de calle 2 quedará en calle 1

        juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ficha]].calle1 = juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ficha]].calle2;

        juego.casillas[juego.jugadores[juego.jugadorTurno].fichas[ficha]].calle2 = Ninguno; //La calle 2 queda vacía
    }

    if (casilla < META) { //Comprobamos que la casilla no esté en la subida a meta

        if (juego.casillas[casilla].calle1 == Ninguno) { //Si no hay ninguna ficha en la calle 1, la ficha se coloca en calle 1

            juego.casillas[casilla].calle1 = /*La ficha que está en calle 1 se iguala a la ficha del turno del jugador */ juego.jugadorTurno;

        }
        else if (juego.casillas[casilla].calle1 == juego.jugadorTurno || (juego.casillas[casilla].calle1 != Ninguno) && esSeguro(casilla)) //Si la ficha es del mismo jugador o 
                                                                                                                                          //está en seguro, la ponemos en calle 2
        {
            juego.casillas[casilla].calle2 = /*La ficha que está en calle 2 se iguala a la ficha del turno del jugador */ juego.jugadorTurno;
        }
        else if ((juego.casillas[casilla].calle1 != Ninguno) && !esSeguro(casilla) && casilla < NUM_CASILLAS) { // Si la ficha no está en seguro nos la comemos

            juego.casillas[casilla].calle2 = juego.casillas[casilla].calle1; // La ficha que había en calle 2 pasa a calle 1
            cout << "Te comiste una ficha de otro jugador!" << endl;
            juego.premio = 20; //Cuenta 20 al comerse otra ficha
            aCasita(juego, casilla); //La manda a casa
            juego.casillas[casilla].calle1 = juego.jugadorTurno;
            //La calle 2 quedaría vacía
            juego.casillas[casilla].calle2 = Ninguno; 

        }

    }
    if (casilla == META) { // Si llega a la casilla 108 (meta)

        cout << "Tu ficha ha llegado a la meta!" << endl;
        juego.premio = 10; //Cuenta 10 al meter la ficha en casa
    }

    juego.jugadores[juego.jugadorTurno].fichas[ficha] = casilla;
}


int lanzardados()
{
    cout << "\n\nPulsa enter para lanzar los dados --> ";
    cin.ignore();
    return rand() % 6 + 1;
}


// Funcion que devuelve el caracter que representa cada color 
char letra(tColor color)
{
    if (color == Amarillo)
        return 'M';

    else if (color == Azul)
        return 'A';

    else if (color == Rojo)
        return 'R';

    else if (color == Verde)
        return 'V';

    else if (color == Ninguno)
        return 'N';

    else if (color == Gris)
        return 'G';
}


void mostrar(const tJuego& juego) {
    int casilla, ficha;
    tColor jug;

    cout << "\x1b[2J\x1b[H"; // Se situa en la esquina superior izquierda
    setColor(Gris);
    cout << endl;

    // Filas con la numeraci�n de las casillas...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i / 10;
    cout << endl;
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << i % 10;
    cout << endl;

    // Borde superior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        cout << '>';
    cout << endl;

    // Primera fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle2);
        if (juego.casillas[i].calle2 != Ninguno)
            cout << primeraEn(juego.jugadores[juego.casillas[i].calle2].fichas, i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    // "Mediana"   
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (esSeguro(i))
            cout << 'o';
        else
            cout << '-';
    cout << endl;


    // Segunda fila de posiciones de fichas...
    for (int i = 0; i < NUM_CASILLAS; i++) {
        setColor(juego.casillas[i].calle1);
        if (juego.casillas[i].calle1 != Ninguno)
            cout << segundaEn(juego.jugadores[juego.casillas[i].calle1].fichas, i) + 1;
        else
            cout << ' ';
        setColor(Gris);
    }
    cout << endl;

    jug = Amarillo;
    // Borde inferior...
    for (int i = 0; i < NUM_CASILLAS; i++)
        if (i == zanataJugador(jug)) {
            setColor(jug);
            cout << "V";
            setColor(Gris);
        }
        else if (i == salidaJugador(jug)) {
            setColor(jug);
            cout << "^";
            setColor(Gris);
            jug = tColor(int(jug) + 1);
        }
        else
            cout << '>';
    cout << endl;

    // Metas y casas...
    for (int i = 0; i < NUM_FICHAS; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug].fichas, 101 + i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug].fichas, 101 + i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug].fichas, 101 + i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
            }
            else if (casilla == salidaJugador(jug)) {
                if (juego.jugadores[jug].fichas[i] == -1) // En casa
                    cout << i + 1;
                else
                    cout << "^";
                jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    // Resto de metas...
    for (int i = 105; i <= 107; i++) {
        casilla = 0;
        jug = Amarillo;
        setColor(jug);
        while (casilla < NUM_CASILLAS) {
            if (casilla == zanataJugador(jug)) {
                ficha = primeraEn(juego.jugadores[jug].fichas, i);
                if (ficha != -1) {
                    cout << ficha + 1;
                    if (cuantasEn(juego.jugadores[jug].fichas, i) > 1) {
                        ficha = segundaEn(juego.jugadores[jug].fichas, i);
                        if (ficha != -1) {
                            cout << ficha + 1;
                        }
                        else
                            cout << "V";
                    }
                    else
                        cout << "V";
                }
                else
                    cout << "VV";
                casilla++;
                jug = tColor(int(jug) + 1);
                setColor(jug);
            }
            else
                cout << ' ';
            casilla++;
        }
        cout << endl;
    }

    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[0] == 108) ? '1' : '.');
        cout << ((juego.jugadores[jug].fichas[1] == 108) ? '2' : '.');
        jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl;
    casilla = 0;
    jug = Amarillo;
    setColor(jug);
    while (casilla < NUM_CASILLAS) {
        cout << ((juego.jugadores[jug].fichas[2] == 108) ? '3' : '.');
        cout << ((juego.jugadores[jug].fichas[3] == 108) ? '4' : '.');
        jug = tColor(int(jug) + 1);
        setColor(jug);
        cout << "               ";
        casilla += 17;
    }
    cout << endl << endl;
    setColor(Gris);
}


void cargar(tJuego& juego) {

    ifstream archivo;
    int jug, cas;

    archivo.open(Archivo);
    if (archivo.is_open()) {
        for (int j = 0; j < NUM_JUGADORES; j++)
            for (int f = 0; f < NUM_FICHAS; f++) {
                archivo >> cas;
                juego.jugadores[j].fichas[f] = cas;
                if ((cas >= 0) && (cas < NUM_CASILLAS))
                    if (juego.casillas[cas].calle1 == Ninguno)
                        juego.casillas[cas].calle1 = tColor(j);
                    else
                        juego.casillas[cas].calle2 = tColor(j);
            }
        archivo >> jug;
        juego.jugadorTurno = tColor(jug);
        archivo.close();
    }
}


