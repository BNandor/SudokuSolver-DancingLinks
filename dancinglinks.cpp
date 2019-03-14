#include <stdio.h>
#include <sstream>
#include <string.h>
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;
class block{
public:
	int S;
	block *U;
	block *D;
	block *R;
	block *L;	
	block *C;
	int N;
};
struct valasztas{
	int val;
	int i,j;
};

#define link_sorszam 729
#define link_oszlopszam 324
#define sudoku_sor 9
#define sudoku_oszlop 9
#define sudoku_ossz_cellaszam 81
#define lehetoseg_cella 9


vector<block*> sorok[link_sorszam];
vector<block*> oszlopok[link_oszlopszam];
vector<block*> header;

vector<valasztas> valasztasok;
int mat[link_sorszam][link_oszlopszam];
int solution_mat[sudoku_sor][sudoku_oszlop];

void put(int i,int j){
	block* b = new block();
	sorok[i].push_back(b);
	oszlopok[j].push_back(b);		
}	

void general_valasztasok(){//0.024s
	
	for (int i = 0; i < sudoku_sor; i++) {
		for (int j = 0; j < sudoku_oszlop; j++) {
			for (int k = 0; k < lehetoseg_cella; k++) {
				valasztas v;v.val=k+1;v.i=i;v.j=j;
				valasztasok.push_back(v);
			}
		}
	}
}

void general_poziciok(){
	for (int i = 0; i < link_sorszam; i++) {
		sorok[i].resize(0);
	}
	for (int i = 0; i < link_oszlopszam; i++) {
		oszlopok[i].resize(0);
	}
	//generate blocks representing positions on field
	int k=0;
	int offset=0;
	for (int i = 0; i < sudoku_ossz_cellaszam; i++) {
		for (int j = 0; j < lehetoseg_cella; j++) {
			put(k,i);		
//			mat[k][i]=1;
			++k;
		}		
	}
	//generate blocks representing the fact that a number appears in a row
	
	offset = sudoku_ossz_cellaszam;
	k=0;
	for (int i = 0; i < sudoku_ossz_cellaszam; i++) {
		for (int j = 0; j < lehetoseg_cella; j++) {

			//mat[k][offset + (valasztasok[k].val-1) + sudoku_oszlop*valasztasok[k].i]=1;
			
			put(k,offset + (valasztasok[k].val-1) + sudoku_oszlop*valasztasok[k].i);
			++k;
		}
	}
	
	offset += sudoku_ossz_cellaszam;
	k=0;
	for (int i = 0; i < sudoku_ossz_cellaszam; i++) {
		for (int j = 0; j < lehetoseg_cella; j++) {
			//mat[k][offset + (valasztasok[k].val-1) + sudoku_oszlop*valasztasok[k].j]=1;
			
			put(k,offset + (valasztasok[k].val-1) + sudoku_oszlop*valasztasok[k].j);
			++k;
		}
	}

	offset += sudoku_ossz_cellaszam;
	k=0;
	for (int i = 0; i < sudoku_ossz_cellaszam; i++) {
		for (int j = 0; j < lehetoseg_cella; j++) {
				
			//mat[k][offset + (valasztasok[k].val-1) + sudoku_oszlop*((valasztasok[k].i/3)*3 + valasztasok[k].j/3)]=1;
			put(k,offset + (valasztasok[k].val-1) + sudoku_oszlop*((valasztasok[k].i/3)*3 + valasztasok[k].j/3));
			++k;
		}
	}
	

}
void lenullaz();
void kiir();
block * root;
void init(){	
	root=new block();
	//link together the blocks row by row
	for (int i = 0; i < link_sorszam; i++) {
		for (int j = 0; j < sorok[i].size()-1; j++) {
			sorok[i][j]->R=sorok[i][j+1];
		}
		sorok[i][sorok[i].size()-1]->R=sorok[i][0];
		for (int j = 1; j < sorok[i].size(); j++) {
			sorok[i][j]->L=sorok[i][j-1];
		}
		sorok[i][0]->L=sorok[i][sorok[i].size()-1];
	}
	//link together the blocks column by column
	for (int i = 0; i < link_oszlopszam; i++) {
//		if(oszlopok[i].size()==0)continue;//unnecessary when mat is filDd up entirely
		for (int j = 0; j < oszlopok[i].size()-1; j++) {
			(oszlopok[i][j])->D=oszlopok[i][j+1];
		}
		oszlopok[i][oszlopok[i].size()-1]->D=oszlopok[i][0];
		for (int j = 1; j < oszlopok[i].size(); j++) {
			oszlopok[i][j]->U=oszlopok[i][j-1];
		}
		oszlopok[i][0]->U=oszlopok[i][oszlopok[i].size()-1];
	}
	
	header.resize(0);	
	//init header
	for (int i = 0; i < link_oszlopszam; i++) {
		block* b  =new block();
		b->S = oszlopok[i].size();

		b->D=oszlopok[i][0];
		b->U=b->D->U;
		b->U->D=b;
		b->D->U=b;
		b->N=i;				
		block* j=b->D;
		while(j!=b){
			j->C=b;
			j=j->D;
		}
		header.push_back(b);
	}
	//link header together by row
	for (int i = 0; i < link_oszlopszam-1; i++) {
		header[i]->R=header[i+1];			
		header[i+1]->L=header[i];
	}
	header[0]->L=root;
	root->L=header[link_oszlopszam-1];
	header[link_oszlopszam-1]->R=root;
	root->R=header[0];

}

void cover(block* c){
	c->R->L=c->L;
	c->L->R=c->R;
	block* i = c->D;
	block* j ;
	while(i!=c){
		j = i->R;
		while(j!=i){
			j->D->U=j->U;	
			j->U->D=j->D;	
			(j->C->S)--;
		j=j->R;		
		}
	i=i->D;
	}
}
void uncover(block*c){
	block* i=c->U;
	block* j;
	while(i!=c){
		j=i->L;
		while(j!=i){
			(j->C->S)++;
			j->D->U=j;
			j->U->D=j;
		j=j->L;
		}
	i=i->U;
	}
	c->R->L=c;
	c->L->R=c;	
}

block* mincolumn(){
	int s=9999;	
	block* c=0;
	block* j = root->R;
	while(j!=root){
		if(j->S < s){
			s=j->S;
			c=j;
		}
	j=j->R;	
	}
	//opt
	if(c==0){
		cout<<"no minimal column found"<<endl;		
	}
return c;
}

block* O[sudoku_ossz_cellaszam];
int to_solve;

void solve(int k);
void beolvas_megold(istream& file){

	to_solve=sudoku_ossz_cellaszam;

	char c;
	vector<int > covered;
	int constraint_index;
	int offset;
	for(int i=0;i<sudoku_sor;i++){
		for(int j=0;j<sudoku_oszlop;j++){
			file>>c;
			if( c != '.'){        
				to_solve--;
				offset = 0;

				constraint_index = i*lehetoseg_cella +j;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	

				offset += sudoku_ossz_cellaszam;
				constraint_index = offset + (c-'0'-1) + sudoku_oszlop*i ;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	
				
				offset += sudoku_ossz_cellaszam;
				constraint_index = offset + (c-'0'-1) + sudoku_oszlop*j;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	
				
				offset += sudoku_ossz_cellaszam;
				constraint_index =offset + (c-'0'-1) + sudoku_oszlop*((i/3)*3 + j/3);
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	
				
				solution_mat[i][j]=c-'0';
			}
		}
	}
	solve(0);
	for (int i = covered.size()-1;i>=0; i--) {
		uncover(header[covered[i]]);
	}

}

bool checkIntegrity(int mat[sudoku_sor][sudoku_oszlop]);
int solution_sum;
int solution_before;
void solve(int k){
	if(root->R == root){		
		solution_sum++;
		block* j;
		for (int i = 0; i < to_solve; i++) {//to solve
			j=O[i];
			int sor,oszlop,ertek;
			int temp;
			do{
				temp = j->C->N;
				if(temp>=sudoku_ossz_cellaszam && temp<2*sudoku_ossz_cellaszam ){//we have got the row constraint
					sor = (temp-sudoku_ossz_cellaszam)/sudoku_sor;
					ertek =(temp-sudoku_ossz_cellaszam)%lehetoseg_cella+1;				      	
				}
				if(temp>=2*sudoku_ossz_cellaszam && temp < 3*sudoku_ossz_cellaszam){//we have got the row constraint
					oszlop = (temp-2*sudoku_ossz_cellaszam)/sudoku_sor;
				}
				j=j->R;
			}while(j!=O[i]);
			solution_mat[sor][oszlop]=ertek;
		}
		#ifdef matrixform
		for (int i = 0; i < sudoku_sor; i++) {
			for (int j = 0;j < sudoku_oszlop; j++) {
				cout<<solution_mat[i][j]<<" ";
			}cout<<endl;
		}
		
		#else	
		for (int i = 0; i < sudoku_sor; i++) {
			for (int j = 0;j < sudoku_oszlop; j++) {
				if(solution_mat[i][j]==0){
					cout<<".";
				}else{
					cout<<solution_mat[i][j];
				}
			}
		}
		#endif
		cout<<endl;
		return;		

	}
	block* c = mincolumn();
	cover(c);
		block* r = c->D;
		block* j ;
#ifndef more
		while(r!=c&& solution_sum-solution_before == 0){
#else			
		while(r!=c ){
#endif			
			O[k]=r;
			j = r->R;
			while(j!=r){
				cover(j->C);
			j=j->R;
			}

			solve(k+1);
			
			r=O[k];
			c=r->C;
			
			j=r->L;
			while(j!=r){
				uncover(j->C);	
			j=j->L;
			}			
		r=r->D;
		}

	uncover(c);
}

int main(int argc, const char *argv[])
{

	if(argc ==1){
		cout<<"usage: "<<argv[0]<<" filename"<<endl;
		cout<<"compile time options :"<<endl;
		cout<<"-Dmatrixform //print solutions in matrix form"<<endl;
		cout<<"-Dintegrity //check if solutions are correct"<<endl;
		cout<<"-Dtotal //print total number of solutions"<<endl;
		cout<<"-Dmore //check all solutions,do not stop at first one"<<endl;
		return -1;
	}
	
	solution_sum=0;
	solution_before=0;
	fstream file; 
	file.open(argv[1]);
	if(!file.is_open()){
		cout<<"could not open file "<<argv[1]<<endl;		
		return -2;
	}	

	general_valasztasok();
	general_poziciok();
	init();		

	string puzzle;
	stringstream ss;
	while(getline(file,puzzle)){
		ss<<puzzle;
		beolvas_megold(ss);
#ifdef integrity		
		if(!checkIntegrity(solution_mat)){
			cout<<"no ok"<<endl;

			for (int i = 0; i < sudoku_sor; i++) {
				for (int j = 0;j < sudoku_oszlop; j++) {
					cout<<solution_mat[i][j]<<" ";
				}cout<<endl;
			}
		}
#endif
		solution_before=solution_sum;
	}
#ifdef total
	cout<<"Total solutions:"<<solution_sum<<endl;	
#endif	
	//free memory
	for (int i = 0; i < link_sorszam; i++) {
		for (int j = 0; j < sorok[i].size(); j++) {
			delete sorok[i][j];		
		}
	}

	for (int i = 0; i < header.size(); i++) {
		delete header[i];
	}
	delete root;
	return 0;

}

void kiir (){
	
	for (int i = 0; i < link_sorszam; i++) {
		for (int j = 0; j < link_oszlopszam; j++) {
			cout<<mat[i][j];
		}cout<<endl;
	}
	
}
void lenullaz(){
	for (int i = 0; i < link_sorszam; i++) {
		for (int j = 0; j < link_oszlopszam; j++) {
			mat[i][j]=0;
		}
	}
	
}

bool checkIntegrity(int mat[sudoku_sor][sudoku_oszlop]){

	int stat[lehetoseg_cella +1]; 

	for(int i=0;i<lehetoseg_cella;i++){
		memset((void*)&stat,0,sizeof(stat));
		for(int j=0;j<lehetoseg_cella;j++){        
			if(mat[i][j]==0)continue;
			stat[mat[i][j]]++;
			if(stat[mat[i][j]]>1){
				return false;
			}
		}                
	}

	for(int i=0;i<lehetoseg_cella;i++){
		memset((void*)&stat,0,sizeof(stat));
		for(int j=0;j<lehetoseg_cella;j++){
			if(mat[j][i]==0)continue;
			stat[mat[j][i]]++;
			if(stat[mat[j][i]]>1){
				return false;
			}
		}
	}
	int boxstat[lehetoseg_cella][10];
	for (int i = 0; i <lehetoseg_cella; i++) {
		memset((void*)&boxstat[i],0,sizeof(boxstat[i]));
	}

	for(int i=0;i<lehetoseg_cella;i++){		
		for(int j=0;j<lehetoseg_cella;j++){
			if(mat[j][i]==0)continue;
			
			boxstat[(i/3)*3 + j/3][mat[j][i]]++;
			if(boxstat[(i/3)*3 + j/3][mat[j][i]]>1){			
				return false;
			}
		}
	}

	return true;
}
