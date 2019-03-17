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
struct choice{
	int val;
	int i,j;
};

#define link_rowcount 729
#define link_columncount 324
#define sudoku_row 9
#define sudoku_column 9
#define sudoku_total_cell_count 81
#define possibility_per_cell 9


vector<block*> rows[link_rowcount];
vector<block*> columns[link_columncount];
vector<block*> header;

vector<choice> choices;
int mat[link_rowcount][link_columncount];
int solution_mat[sudoku_row][sudoku_column];

void put(int i,int j){
	block* b = new block();
	rows[i].push_back(b);
	columns[j].push_back(b);		
}	

void generate_choices(){//0.024s
	
	for (int i = 0; i < sudoku_row; i++) {
		for (int j = 0; j < sudoku_column; j++) {
			for (int k = 0; k < possibility_per_cell; k++) {
				choice v;v.val=k+1;v.i=i;v.j=j;
				choices.push_back(v);
			}
		}
	}
}

void generate_positions(){
	for (int i = 0; i < link_rowcount; i++) {

	}
	for (int i = 0; i < link_columncount; i++) {
		columns[i].resize(0);
	}
	//generate blocks representing positions on field
	int k=0;
	int offset=0;
	for (int i = 0; i < sudoku_total_cell_count; i++) {
		for (int j = 0; j < possibility_per_cell; j++) {
			put(k,i);		
//			mat[k][i]=1;
			++k;
		}		
	}
	//generate blocks representing the fact that a number appears in a row
	
	offset = sudoku_total_cell_count;
	k=0;
	for (int i = 0; i < sudoku_total_cell_count; i++) {
		for (int j = 0; j < possibility_per_cell; j++) {

			//mat[k][offset + (choices[k].val-1) + sudoku_column*choices[k].i]=1;
			
			put(k,offset + (choices[k].val-1) + sudoku_column*choices[k].i);
			++k;
		}
	}
	
	offset += sudoku_total_cell_count;
	k=0;
	for (int i = 0; i < sudoku_total_cell_count; i++) {
		for (int j = 0; j < possibility_per_cell; j++) {
			//mat[k][offset + (choices[k].val-1) + sudoku_column*choices[k].j]=1;
			
			put(k,offset + (choices[k].val-1) + sudoku_column*choices[k].j);
			++k;
		}
	}

	offset += sudoku_total_cell_count;
	k=0;
	for (int i = 0; i < sudoku_total_cell_count; i++) {
		for (int j = 0; j < possibility_per_cell; j++) {
				
			//mat[k][offset + (choices[k].val-1) + sudoku_column*((choices[k].i/3)*3 + choices[k].j/3)]=1;
			put(k,offset + (choices[k].val-1) + sudoku_column*((choices[k].i/3)*3 + choices[k].j/3));
			++k;
		}
	}
	

}
void nullify();
void print();
block * root;
void init(){	
	root=new block();
	//link together the blocks row by row
	for (int i = 0; i < link_rowcount; i++) {
		for (int j = 0; j < rows[i].size()-1; j++) {
			rows[i][j]->R=rows[i][j+1];
		}
		rows[i][rows[i].size()-1]->R=rows[i][0];
		for (int j = 1; j < rows[i].size(); j++) {
			rows[i][j]->L=rows[i][j-1];
		}
		rows[i][0]->L=rows[i][rows[i].size()-1];
	}
	//link together the blocks column by column
	for (int i = 0; i < link_columncount; i++) {
//		if(columns[i].size()==0)continue;//unnecessary when mat is filDd up entirely
		for (int j = 0; j < columns[i].size()-1; j++) {
			(columns[i][j])->D=columns[i][j+1];
		}
		columns[i][columns[i].size()-1]->D=columns[i][0];
		for (int j = 1; j < columns[i].size(); j++) {
			columns[i][j]->U=columns[i][j-1];
		}
		columns[i][0]->U=columns[i][columns[i].size()-1];
	}
	
	header.resize(0);	
	//init header
	for (int i = 0; i < link_columncount; i++) {
		block* b  =new block();
		b->S = columns[i].size();

		b->D=columns[i][0];
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
	for (int i = 0; i < link_columncount-1; i++) {
		header[i]->R=header[i+1];			
		header[i+1]->L=header[i];
	}
	header[0]->L=root;
	root->L=header[link_columncount-1];
	header[link_columncount-1]->R=root;
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

block* O[sudoku_total_cell_count];
int to_solve;

void solve(int k);
void read_and_solve(istream& file){

	to_solve=sudoku_total_cell_count;

	char c;
	vector<int > covered;
	int constraint_index;
	int offset;
	for(int i=0;i<sudoku_row;i++){
		for(int j=0;j<sudoku_column;j++){
			file>>c;
			if( c != '.'){        
				to_solve--;
				offset = 0;

				constraint_index = i*possibility_per_cell +j;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	

				offset += sudoku_total_cell_count;
				constraint_index = offset + (c-'0'-1) + sudoku_column*i ;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	
				
				offset += sudoku_total_cell_count;
				constraint_index = offset + (c-'0'-1) + sudoku_column*j;
				cover(header[constraint_index]);
				covered.push_back(constraint_index);	
				
				offset += sudoku_total_cell_count;
				constraint_index =offset + (c-'0'-1) + sudoku_column*((i/3)*3 + j/3);
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

bool checkIntegrity(int mat[sudoku_row][sudoku_column]);
int solution_sum;
int solution_before;
void solve(int k){
	if(root->R == root){		
		solution_sum++;
		block* j;
		for (int i = 0; i < to_solve; i++) {//to solve
			j=O[i];
			int row,column,ertek;
			int temp;
			do{
				temp = j->C->N;
				if(temp>=sudoku_total_cell_count && temp<2*sudoku_total_cell_count ){//we have got the row constraint
					row = (temp-sudoku_total_cell_count)/sudoku_row;
					ertek =(temp-sudoku_total_cell_count)%possibility_per_cell+1;				      	
				}
				if(temp>=2*sudoku_total_cell_count && temp < 3*sudoku_total_cell_count){//we have got the row constraint
					column = (temp-2*sudoku_total_cell_count)/sudoku_row;
				}
				j=j->R;
			}while(j!=O[i]);
			solution_mat[row][column]=ertek;
		}
		#ifdef matrixform
		for (int i = 0; i < sudoku_row; i++) {
			for (int j = 0;j < sudoku_column; j++) {
				cout<<solution_mat[i][j]<<" ";
			}cout<<endl;
		}
		
		#else	
		for (int i = 0; i < sudoku_row; i++) {
			for (int j = 0;j < sudoku_column; j++) {
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

	generate_choices();
	generate_positions();
	init();		

	string puzzle;
	stringstream ss;
	while(getline(file,puzzle)){
		ss<<puzzle;
		read_and_solve(ss);
#ifdef integrity		
		if(!checkIntegrity(solution_mat)){
			cout<<"no ok"<<endl;

			for (int i = 0; i < sudoku_row; i++) {
				for (int j = 0;j < sudoku_column; j++) {
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
	for (int i = 0; i < link_rowcount; i++) {
		for (int j = 0; j < rows[i].size(); j++) {
			delete rows[i][j];		
		}
	}

	for (int i = 0; i < header.size(); i++) {
		delete header[i];
	}
	delete root;
	return 0;

}

void print (){
	
	for (int i = 0; i < link_rowcount; i++) {
		for (int j = 0; j < link_columncount; j++) {
			cout<<mat[i][j];
		}cout<<endl;
	}
	
}
void nullify(){
	for (int i = 0; i < link_rowcount; i++) {
		for (int j = 0; j < link_columncount; j++) {
			mat[i][j]=0;
		}
	}
	
}

bool checkIntegrity(int mat[sudoku_row][sudoku_column]){

	int stat[possibility_per_cell +1]; 

	for(int i=0;i<possibility_per_cell;i++){
		memset((void*)&stat,0,sizeof(stat));
		for(int j=0;j<possibility_per_cell;j++){        
			if(mat[i][j]==0)continue;
			stat[mat[i][j]]++;
			if(stat[mat[i][j]]>1){
				return false;
			}
		}                
	}

	for(int i=0;i<possibility_per_cell;i++){
		memset((void*)&stat,0,sizeof(stat));
		for(int j=0;j<possibility_per_cell;j++){
			if(mat[j][i]==0)continue;
			stat[mat[j][i]]++;
			if(stat[mat[j][i]]>1){
				return false;
			}
		}
	}
	int boxstat[possibility_per_cell][10];
	for (int i = 0; i <possibility_per_cell; i++) {
		memset((void*)&boxstat[i],0,sizeof(boxstat[i]));
	}

	for(int i=0;i<possibility_per_cell;i++){		
		for(int j=0;j<possibility_per_cell;j++){
			if(mat[j][i]==0)continue;
			
			boxstat[(i/3)*3 + j/3][mat[j][i]]++;
			if(boxstat[(i/3)*3 + j/3][mat[j][i]]>1){			
				return false;
			}
		}
	}

	return true;
}
