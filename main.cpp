// GRENIER Alexandre
// Projet C++ Morse
// Janvier 2021

#include <cmath> // Pour génerer des signaux sinusoïdaux
#include <fstream> // Pour gérer les fichiers
#include <iostream> // Pour gérer les fichiers
#include <map> // Pour gérer les dictionnaire
#include <stdio.h> // Pour gérer les fichiers
#include <vector>
using namespace std;

// Définition de constantes globales
constexpr double two_pi = 6.283185307179586476925286766559;
constexpr double max_amplitude = 32760; // volume maximal
double hz = 44100; // échantillonage
double frequency = 1174.66 ; // Do5

// Définition du code morse
map<char, string> code_morse = {
    {'A', ".-"},
    { 'B', "-..." },
    { 'C', "-.-." },
    { 'D', "-.." },
    { 'E', "." },
    { 'F', "..-." },
    { 'G', "--." },
    { 'H', "...." },
    { 'I', ".." },
    { 'J', ".---" },
    { 'K', "-.-" },
    { 'L', ".-.." },
    { 'M', "--" },
    { 'N', "-." },
    { 'O', "---" },
    { 'P', ".--." },
    { 'Q', "--.-" },
    { 'R', ".-." },
    { 'S', "..." },
    { 'T', "-" },
    { 'U', "..-" },
    { 'V', "...-" },
    { 'W', ".--" },
    { 'X', "-..-" },
    { 'Y', "-.--" },
    { 'Z', "--.." },
    { '1', ".----" },
    { '2', "..---" },
    { '3', "...--" },
    { '4', "....-" },
    { '5', "....." },
    { '6', "-...." },
    { '7', "--..." },
    { '8', "---.." },
    { '9', "----." },
    { '0', "-----" },
    { ' ', "/" }
};

map<string, string> morse_to_text = {
    {".-", "A"},
    {"-...", "B"},
    {"-.-.", "C"},
    {"-..", "D"},
    {".", "E" },
    {"..-.", "F" },
    {"--.", "G" },
    {"....", "H" },
    {"..", "I" },
    {".---", "J" },
    {"-.-", "K" },
    {".-..", "L" },
    {"--", "M" },
    {"-.", "N" },
    {"---", "O" },
    {".--.", "P" },
    {"--.-", "Q" },
    {".-.", "R" },
    {"...", "S" },
    {"-", "T" },
    {"..-", "U" },
    {"...-", "V" },
    {".--", "W" },
    {"-..-", "X" },
    {"-.--", "Y" },
    {"--..", "Z" },
    {".----", "1" },
    {"..---", "2" },
    {"...--", "3" },
    {"....-", "4" },
    {".....", "5" },
    {"-....", "6" },
    {"--...", "7" },
    {"---..", "8" },
    {"----.", "9" },
    {"-----", "0" },
    {"/", " " }
};

// Conversion lettre en morse
string conversion_lettre_morse(string texte)
{
    string code = "";
    for(unsigned int b = 0; b < texte.length(); b++)
    {
        if ( code_morse.find(texte[b]) == code_morse.end() ) 
        {
            return "error";
        }
        code = code + code_morse[texte[b]] + " ";
    }
    return code;
}


// Utilisation de la méthode du petit boutisme pour l'écriture en binaire (nécessaire au format .wav)
namespace little_endian_io
{
  template <typename Word>
  // Définition de write_word avec la méthode du petit boutisme
  std::ostream& write_word( std::ostream& outs, Word value, unsigned size = sizeof( Word ) )
  {
    for (; size; --size, value >>= 8)
      outs.put( static_cast <char> (value & 0xFF) );
    return outs;
  }
}
using namespace little_endian_io;

// Ajoute au fichier un son de fréquence hz et de durée seconds (permet de faire les ti et les tah)
void sound(double seconds, ofstream& fichier)
{
    int N = hz * seconds; // nombre total d'échantillons
    for (int n = 0; n < N; n++)
    {
        int intensity;
        double amplitude = max_amplitude/5;
        double value     = sin( (two_pi * n * frequency) / hz );
        if((int)(amplitude * value) == 0)
        {
            intensity = 1;
        }
        else
        {
            intensity = (int)(amplitude * value);
        }
        write_word(fichier, (int)(amplitude * value), 2 );
        //write_word(fichier, (int)(amplitude * value), 2 );
    }
}

// Ajoute au fichier un silence de durée seconds (permet de faire les ti et les tah)
void silence(double seconds, ofstream& fichier)
{
    int N = hz * seconds;
    for (int n = 0; n < N; n++)
    {
        write_word( fichier, 0, 2 );
        write_word( fichier, 0, 2 );
    }
}
// transformation des symboles en audio
void encode(string code, ofstream& f)
{
    for(int b = 0; b < code.size();)
    {
         if(code[b] == '.')
         {
             sound(0.07,f);
             silence(0.07,f);
         }
         if(code[b]=='-')
         {
             sound(0.21,f);
             silence(0.07,f);
         }
         if(code[b]==' ')
         {
             silence(0.21,f);
         }
         if(code[b]=='/')
         {
             silence(0.5,f);
         }
    b++;
    }
}

// fonction pour séparer les mots et les lettres
size_t split(const std::string &txt, std::vector<std::string> &strs, char ch)
{
    size_t pos = txt.find( ch );
    size_t initialPos = 0;
    strs.clear();

    while( pos != std::string::npos ) {
        strs.push_back( txt.substr( initialPos, pos - initialPos ) );
        initialPos = pos + 1;

        pos = txt.find( ch, initialPos );
    }
    strs.push_back( txt.substr( initialPos, std::min( pos, txt.size() ) - initialPos + 1 ) );

    return strs.size();
}

void encodage()
{
    string nom_fichier;
    string message;
    string message_morse = "error";

    cout << "*** Conversion TEXTE => MORSE AUDIO ***" << endl;
    cout << "Choisissez un nom de fichier :" << endl;
    cin >> nom_fichier;
    while(message_morse == "error")
    {
        cout << "Entrez le message (majuscules et chiffres seulement) :" << endl;
        cin.clear();
        cin.ignore();
        getline(cin, message);
        message_morse = conversion_lettre_morse(message);
        if(message_morse == "error")
        {
            cout << "/!\\ Seules les majuscules et les chiffres sont autorisées /!\\" << endl;    
        }
    }

    cout << message_morse << endl;

    ofstream f( nom_fichier + ".wav", ios::binary );

    // Header
    f << "RIFF----WAVEfmt ";     
    write_word( f,     16, 4 );  
    write_word( f,      1, 2 );  
    write_word( f,      1, 2 );  
    write_word( f,  44100, 4 ); 
    write_word( f, 176400, 4 );  
    write_word( f,      4, 2 );  
    write_word( f,     16, 2 ); 

    // Ajout de la taille des données
    size_t data_chunk_pos = f.tellp();
    f << "data----";  // (chunk size to be filled in later)
    

    encode(message_morse,f);

    
    // Ajout de la taille totale des données
    size_t file_length = f.tellp();

    f.seekp( data_chunk_pos + 4 );
    write_word( f, file_length - data_chunk_pos + 8 );

    f.seekp( 0 + 4 );
    write_word( f, file_length - 8, 4 ); 
}

void decodage()
{
    string nom_fichier;


    cout << "*** Conversion MORSE AUDIO => TEXTE ***" << endl;
    cout << "Entrez le nom du fichier :" << endl;
    cin >> nom_fichier;
    nom_fichier = nom_fichier + ".wav";

    FILE * pFile;
    long lSize = 1;
    size_t result;
    char* buffer = new char[2];
    int n = 0;
    // ouverture du fichier
    pFile = fopen ( nom_fichier.c_str() , "rb" );
    if (pFile==NULL) {fputs ("File error",stderr); exit (1);}
    // evaluation de la taille du fichier
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile) - 44;
    int amplitudes[(int)(lSize/2)-1];
    while(n<lSize/2)
    {
        fseek (pFile , 44+2*n , SEEK_SET); // on parcours le fichier à partir de la fin du header de 2 en 2 
        fread (buffer,2,1,pFile); // on relève 2 bytes
        int a = buffer[0];
        int b = buffer[1];
        amplitudes[n]= (a << 8 | b) !=0; // on vérifie si la valeur sur les 2 bytes est nulle ou non
        n++;
    }
    int k=0;

    std::vector<int> occurences {0};
    std::vector<int> valeurs {0};
    // on compte le nombre d'occurences de chaque 1 et 0 afin de derminer s'il sagit de ti, tah, séparation de lettres ou de mots 
    while(k<lSize/2)
    {
        if(valeurs[valeurs.size()-1]==amplitudes[k])
        {
            occurences[occurences.size()-1] = occurences[occurences.size()-1] + 1;
        }
        else
        {
            valeurs.push_back(amplitudes[k]);
            occurences.push_back(1);
        }
        k++;
    }

    
    string decode_morse = "";
    string ti = ".";
    string tah = "-";
    string espace = " ";
    string mot = " / ";
    // on transforme les suites de 1 ou de 0 en caractères morse
    for(int b = 0; b < valeurs.size();)
    {
         if(valeurs[b] == 1 && occurences[b] > 2900 && occurences[b] < 3200)
         {
            decode_morse = decode_morse + ti;
         }
         if(valeurs[b] == 1 && occurences[b] > 9000 && occurences[b] < 9400)
         {
            decode_morse = decode_morse + tah; 
         }
         if(valeurs[b] == 0 && occurences[b] > 24000 && occurences[b] < 25000)
         {
            decode_morse = decode_morse + espace;
         }
         if(valeurs[b] == 0 && occurences[b] > 87000 && occurences[b] < 90000)
         {
            decode_morse = decode_morse + mot;
         }
    b++;
    }
    // on sépare les mots puis les lettres et on transcrit chaque lettre
    string message_final = "";
    std::vector<std::string> mots;
    split(decode_morse,mots,'/');
    for(int b = 0; b < mots.size();)
    {
         std::vector<std::string> lettres;
         split(mots[b],lettres, ' ');
         for(int z= 0; z < lettres.size();)
         {
             message_final = message_final + morse_to_text[lettres[z]];
             z++;
         }
         message_final = message_final + " "; 
    b++;
    }

    cout << decode_morse << endl;
    cout << message_final << endl;
    fclose (pFile);
}

int main()
{
    string mode;
    cout << "Choisissez un mode Encodage (E) / Decodage (D)" << endl;
    cin >> mode;
    if(mode == "E")
    {
        encodage();
    }
    if(mode == "D")
    {
        decodage();
    }
    else
    {
        return 0;
    }
    
    return 0;
}