#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <windows.h>

using namespace std;

//Guarda o inicio, o meio e o fim do .txt do automoto
struct tags_inicio_e_fim{
    string inicio = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?><!--Criado com Jefferson 1.0--><structure>&#13;";
    string inicio_2 = "\n\t<type>fa</type>&#13;";
    string inicio_3 = "\n\t<automaton>&#13;";
    string inicio_4 = "\n\t\t<!--The list of states.-->&#13;";
    string meio = "\n\t\t<!--The list of transitions.-->&#13;";
    string rotaX = "\n\t\t\t<x>0</x>&#13;";
    string rotaY = "\n\t\t\t<y>0</y>&#13;";
    string fim_2 = "\n\t</automaton>&#13;";
    string fim_3 = "\n</structure>";
};
struct coordenadas{
    string rotax;
    string rotay;
};
//Guarda o tipo dos estados e � definido de acordo com a bandeira de cada um, onde a bandeira est� salva na struct Estados
struct tags_estados{
    string inicial = "\n\t\t\t<initial/>&#13;";
    string finall = "\n\t\t\t<final/>&#13;";
    string transa = "\t\t<transition>&#13;";
};
//Guarda as transi��es e estados j� montadas a partir das structs que vieram do pegaEstados
struct tag_nome_id{
    string id;
    string cond;
    string transition = "\t\t</state>&#13;";
};
//Guarda todas as transi��es vindas do automoto do usu�rio
struct Transa{
    char id[1];
    char to[1];
    char read[1];
    int quant;
    int readQuant;
};
//Guarda todos os estados vindo do automoto do usu�rio
struct Estados {
    char id[1];
    char nome[1];
    int bandeira = 0; // 1 = Estado inicial !! 2 = Estado final !! 3 = Estado inicial e final !! 0 = Estado comum
};
//Id_Finais guarda o id final para ser utilizando na estrela
struct id_Finais{
    char id[1];
};

// A fun��o pegaEstados ela separa e guarda todos os estados e todas as transi��es
int pegaEstados(string diretorio, Estados *estado, Transa *transa, coordenadas *coord){
    int tam = diretorio.length();
    char drt[tam +1];
    int c = 0;
    int flag = 0;
    int aux;
    transa->quant = 0;
    transa->readQuant = 0;
    string qualquer = "";

    strcpy(drt, diretorio.c_str());

    ifstream arq;
    char lines[5000];
    char lines2[5001];
    arq.open(drt);

    if(!arq.is_open())
        return 0;


    while(arq.getline(lines,102)){
        qualquer += lines;
        qualquer += '\n';
    }

    //Pega o ID dos estados
    for(int i = 175; i<=qualquer.length(); i++){
        if(qualquer[i] == '<' && qualquer[i+1] == 'x' && qualquer[i+2] == '>'){
            aux = i+2;
            while(qualquer[++aux] != '.'){
                coord[c].rotax += qualquer[aux];
                // coord[c].rotax = "0";
            }
        }
        else if(qualquer[i] == '<' && qualquer[i+1] == 'y' && qualquer[i+2] == '>'){
            aux = i+2;
            while(qualquer[++aux] != '.'){
                coord[c].rotay += qualquer[aux];
                // coord[c].rotay = "0";
            }
        }
        //Pega os estados somente inicias
        if(qualquer[i] == 'i' && qualquer[i+1] == 'd' && qualquer[i+2] == '=' && qualquer[i+3] == '"'){
            estado[c].id[1] = qualquer[i+4];
            flag = 1;
            c++;
        }
        //Pega os estados inicias e finais
        if(qualquer[i] == 'i' && qualquer[i+1] == 'n' && qualquer[i+2] == 'i'){
            estado[c-1].bandeira = 1;
            if(qualquer[i+19] == 'f'){
                estado[c-1].bandeira = 3;
                flag = 2;
            }
        }
        //Pega os estados somente finais
        else if(qualquer[i] == 'f' && qualquer[i+1] == 'i' && qualquer[i+2] == 'n' && flag != 2){
            estado[c-1].bandeira = 2;
        }
        //Pega ids das transi��es
        if(qualquer[i] == '<' && qualquer[i+1] == 'f' && qualquer[i+2] == 'r' && qualquer[i+3] == 'o'){
            transa[transa->quant].id[1] = qualquer[i+6];
            transa[transa->quant].to[1] = qualquer[i+27];
            transa[transa->quant].read[1] = qualquer[i+48]; //Provavelmente o erro est� aqui todo
            transa->quant++;
        }
    }
    //Preenche os estados "vazios" ou comuns
    for(int i = 0; i<c ; i++){
        if(estado[i].bandeira != 1 && estado[i].bandeira != 2 && estado[i].bandeira != 3){
            estado[i].bandeira = 0;
        }
    }
    arq.close();
    return c;

}
//A fun��o criaComplemento cria as estruturas para o complemento a partir da fun��o pegaEstados
int criaComplemento(Estados *estado, int c, tag_nome_id *nome, tags_estados *i_f_t, Transa *transa, tag_nome_id *trans){
    //Cria os estados
    for(int i = 0; i<c; i++){
        nome[i].id = "\t\t<state id=\"";
        nome[i].id += estado[i].id[1];
        nome[i].id += "\" ";
        nome[i].id += "name=\"q";
        nome[i].id += estado[i].id[1];
        nome[i].id += "\">&#13;";
        if(estado[i].bandeira == 1){
            nome[i].cond += i_f_t[i].inicial;
            nome[i].cond += i_f_t[i].finall;
        }
        else if(estado[i].bandeira == 3){
            nome[i].cond += i_f_t[i].inicial;
        }
        else if(estado[i].bandeira == 0){
            nome[i].cond += i_f_t[i].finall;
        }
    }

    //Cria as transi��es
    for(int i = 0; i<transa->quant; i++){
        trans[i].id = "\n\t\t<transition>&#13;\n";
        trans[i].id += "\t\t\t<from>";
        trans[i].id += transa[i].id[1];
        trans[i].id += "</from>&#13;\n";
        trans[i].id += "\t\t\t<to>";
        trans[i].id += transa[i].to[1];
        trans[i].id += "</to>&#13;\n";
        if(transa[i].read[1] == '>'){ //Provavelmente o erro est� aqui todo
            trans[i].id += "\t\t\t<read/>&#13;\n";
        }
        else{
            trans[i].id += "\t\t\t<read>";
            trans[i].id += transa[i].read[1];
            trans[i].id += "</read>&#13;\n";
        }
        trans[i].id += "\t\t</transition>&#13;";
    }
    return c;
}
//A fun��o criaEstrela cria as estruturas para a estrela a partir da fun��o pegaEstados
int criaEstrela(Estados *estado, int c, tag_nome_id *nome, tags_estados *i_f_t, Transa *transa, tag_nome_id *trans, id_Finais *finais, tag_nome_id *lambidas, int &quant_finais){
    char alfa[1]; // Guarda o ID do estado inicial para a transi��es lambidas irem
    string omega; // Guarda o ID seguinte ao �ltimo ID existente no automoto, para ser o novo ID inicial
    omega = std::to_string(c); //Converte o c para ser o �ltimo ID //RedWarning pode acontecer aqui, normal
    c += 1;
    //Cria os estados
    for(int i = 0; i<c; i++){
        nome[i].id = "\t\t<state id=\"";
        if((i+1) == c){
            nome[i].id += omega;
            estado[i].bandeira = 1;
        }
        else{
            nome[i].id += estado[i].id[1];
        }
        nome[i].id += "\" ";
        nome[i].id += "name=\"q";
        if((i+1) == c){
            nome[i].id += omega;
        }
        else{
            nome[i].id += estado[i].id[1];
        }
        nome[i].id += "\">&#13;";
        if(estado[i].bandeira == 1){
            if((i+1) == c){
                nome[i].cond += i_f_t[i].inicial;
            }
            else{
                alfa[1] = estado[i].id[1];
            }
        }
        else if(estado[i].bandeira == 2){
            nome[i].cond += i_f_t[i].finall;
            finais[quant_finais].id[1] = estado[i].id[1];
            quant_finais++;
        }
        else if(estado[i].bandeira == 3){
            nome[i].cond += i_f_t[i].finall;
            alfa[1] = estado[i].id[1];
            finais[i].id[1] = estado[i].id[1];
            quant_finais++;
        }
    }
    //Cria as transi��es
    for(int i = 0; i<transa->quant; i++){
        trans[i].id = "\n\t\t<transition>&#13;\n";
        trans[i].id += "\t\t\t<from>";
        if((i+1) == transa->quant){
            trans[i].id += omega;
        }
        else{
            trans[i].id += transa[i].id[1];
        }
        trans[i].id += "</from>&#13;\n";
        trans[i].id += "\t\t\t<to>";
        if((i+1) == transa->quant){
            trans[i].id += alfa[1];
            transa[i].read[1] = '>';
        }
        else{
            trans[i].id += transa[i].to[1];
        }
        trans[i].id += "</to>&#13;\n";
        //Transi��o lambida
        if(transa[i].read[1] == '>'){
            trans[i].id += "\t\t\t<read/>&#13;\n";
        }
        else{
            trans[i].id += "\t\t\t<read>";
            trans[i].id += transa[i].read[1]; //Provavelmente o erro est� aqui todo
            trans[i].id += "</read>&#13;\n";
        }
        trans[i].id += "\t\t</transition>&#13;";
    }

    //Cria as transi��es lambidas
    for(int i = 0; i<quant_finais; i++){
        lambidas[i].id = "\n\t\t<transition>&#13;\n";
        lambidas[i].id += "\t\t\t<from>";
        lambidas[i].id += finais[i].id[1];
        lambidas[i].id += "</from>&#13;\n";
        lambidas[i].id += "\t\t\t<to>";
        lambidas[i].id += alfa[1];
        lambidas[i].id += "</to>&#13;\n";
        //Transi��o lambida
        lambidas[i].id += "\t\t\t<read/>&#13;\n";
        lambidas[i].id += "\t\t</transition>&#13;";
    }

return c;
}
//A fun��o criaAutomoto salva e cria o automoto propriamento dito, nos padr�es do .jff
int criaAutomoto(string nome_automoto,tag_nome_id *nome, tag_nome_id *trans, tags_inicio_e_fim tagss, int c, Transa *transa, tag_nome_id *lambidas, int &quant_finais, int op, coordenadas *coord){
    ofstream arq;
    if(op == 2){
        nome_automoto = "Complemento.jff";
    }
    else if(op == 3){
        nome_automoto = "Estrela.jff";
    }
    else
        return 0;
    int tam = nome_automoto.length();
    int cont = 0;
    char drt[tam +1];
    strcpy(drt, nome_automoto.c_str());
    arq.open(drt);
    if(!arq.is_open())
        return 0;
    arq << tagss.inicio;
    arq << tagss.inicio_2;
    arq << tagss.inicio_3;
    arq << tagss.inicio_4;
    arq << "\n";
    //Passa os estados para o arquivo
    for(int i = 0; i<c; i++){
        arq << nome[i].id;
        if(op == 2){
            arq << "\n\t\t\t<x>"+coord[i+1].rotax+"</x>&#13;";
            arq << "\n\t\t\t<y>"+coord[i+1].rotay+"</y>&#13;";
        } else if(op == 3){
            if(++cont == c){
                arq << "\n\t\t\t<x>0</x>&#13;";
                arq << "\n\t\t\t<y>0</y>&#13;";
            }else{
                arq << "\n\t\t\t<x>"+coord[i+1].rotax+"</x>&#13;";
                arq << "\n\t\t\t<y>"+coord[i+1].rotay+"</y>&#13;";
            }
        }
        if(nome[i].cond != "\t\t</state>&#13;")
            arq << nome[i].cond + "\n";
        else
            arq << "\n" + nome[i].cond + "\n";
        if(i + 1 == c)
            arq << nome[i].transition;
        else
            arq << nome[i].transition + "\n";
    }

    arq << tagss.meio;
    //Passa as transi��es pro arquivo
    if(op == 3)
        transa->quant += 1;
    for(int i = 0; i<transa->quant; i++){
        arq << trans[i].id;

    }
    //Passa as transi��es lambidas pro arquivo
    for(int i = 0; i<quant_finais; i++){
        arq << lambidas[i].id;

    }

    arq << tagss.fim_2;
    arq << tagss.fim_3;


    arq.close();


return 0;}

void menu(string nome_arquivo){
    cout<< "Smart Au-tomate: Gerador de complemento e estrela"<<endl;
    cout << "O arquivo .jff precisa estar na pasta deste programa!" << endl;
        

    cout << "1 - Novo automoto(automoto atual: '" << nome_arquivo << "' )" << endl;
    cout << "2 - Complemento" << endl;
    cout << "3 - Estrela" << endl;
    cout << "4 - Abrir arquivo" << endl;
    cout << "5 - Fechar programa" << endl;
    cout << "Escolha: ";
}

void printTomate(){
    cout<< "  ____    __  __      _      ____    _____ "<< endl;
    cout<< " / ___|  |  \/  |    / \    |  _ \  |_   _|"<< endl;
    cout<< " \___ \  | |\/| |   / _ \   | |_) |   | |  "<< endl;
    cout<< "  ___) | | |  | |  / ___ \  |  _ <    | |  "<< endl;
    cout<< " |____/  |_|  |_| /_/   \_\ |_| \_\   |_|  "<< endl;
    cout<< "     _      _   _           _____    ___    __  __      _      _____   _____ "<< endl;
    cout<< "    / \    | | | |         |_   _|  / _ \  |  \/  |    / \    |_   _| | ____|"<< endl;
    cout<< "   / _ \   | | | |  _____    | |   | | | | | |\/| |   / _ \     | |   |  _|  "<< endl;
    cout<< "  / ___ \  | |_| | |_____|   | |   | |_| | | |  | |  / ___ \    | |   | |___ "<< endl;
    cout<< " /_/   \_\  \___/            |_|    \___/  |_|  |_| /_/   \_\   |_|   |_____|"<< endl; 
    Sleep(1000);
    system("cls");                                                                            
}

int main()
{
    system("color 0E");
    Estados estado[100]; // estado guarda todos os estados e ids vindos do automoto do usu�rio
    Transa transa[100]; // transa � diferente de trans, onde Transa guarda as transi��es que vieram do automoto do usu�rio
    id_Finais finais[100]; // finais guarda todos os ids finais do aumoto
    tags_inicio_e_fim textos; // Aqui, est� guardado os primeiros e �ltimos textos do .txt do jff
    tags_estados textos_estados;
    tag_nome_id nome[100]; // nome guarda a linha da sequ�ncia do id e do nome do id
    tag_nome_id trans[100]; // trans guarda as transi��es que n�o s�o lambidas
    tag_nome_id lambidas[100]; //Tag nome id lambidas guarda as transi��es lambidas, os ids e suas condi��es
    tags_estados i_f_d[100]; //Tag estados guarda o tipo do estado, se � final, inicial, ou nenhum dos dois
    coordenadas coord[100];
    int quant_finais = 0; // Quantidade de estados finais
    int c; // c � quantidade de estados do automoto escolhido pelo usu�rio no total
    int op; // Escolha do usu�rio, 1 para complemento e 2 para estrela
    string nome_arquivo, formato, diret;

    printTomate();

    while(op != 5){
        menu(nome_arquivo);
        cin >> op;
        fflush(stdin);
        diret = nome_arquivo + ".jff";

        if(op == 1){
            cout << "Digite exatamente o nome do arquivo: ";
            getline( cin, nome_arquivo);
        }
        if(op == 2){
            c = criaComplemento(estado,pegaEstados(diret,estado,transa, coord),nome,i_f_d,transa,trans);
        }
        if(op == 3){
            c = criaEstrela(estado,pegaEstados(diret,estado,transa, coord),nome,i_f_d,transa,trans, finais, lambidas, quant_finais);
        }
        if(op == 2 || op == 3){
            criaAutomoto(nome_arquivo ,nome ,trans ,textos ,c , transa, lambidas, quant_finais, op, coord);
            if(op == 2)
                system("complem.bat");
            else if(op == 3)
                system("estrel.bat");
            for(int i = 0 ; i < 100; i++){
                coord[i] = coordenadas();
                estado[i] = Estados();
                transa[i] = Transa();
                trans[i] = tag_nome_id();
                lambidas[i] = tag_nome_id();
                nome[i] = tag_nome_id();
                i_f_d[i] = tags_estados();
                finais[i] = id_Finais();
            }
            textos = tags_inicio_e_fim();
            textos_estados = tags_estados();
            quant_finais = 0;
        }
        if(op > 5 || op < 0)
            cout << "Comando invalido" << endl;
        system("cls");
    }
    return 0;
}

