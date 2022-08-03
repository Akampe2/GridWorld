
#ifndef _GRID_WORLD_H
#define _GRID_WORLD_H

#include <vector>
#include <iostream>

using std::vector;


class GridWorld {

  private:

    unsigned nrows =0;
    unsigned ncols =0;

    int numOfDistr;//keep track of districts created


    //Attributes of a person
    struct person {
        int row;//location of person
        int col;
        int id;
        person *prev;
        person *next;
        bool living;

        //Initially contain no information
        person(int row, int col, int id) {
            this -> row = row;
            this -> col = col;
            this -> id = id;
            prev = nullptr;
            next = nullptr;
            living = true;
        }
        bool isAlive(){
            return living;
        }

        int getRow(){
            return row;
        }

        int getCol(){
            return col;
        }

        int getID(){
            return id;
        }

        void updateLocation(int row, int col){
            this -> row = row;
            this -> col = col;
        }
    };


    //Regional list of people
    struct District{

        person* front;//Declaration for where the first vector index will point
        person* back;//Declaration for where the last vector index will point
        int districtPopul;//keep track of population
        int row; //storing location of district
        int col;

        //Initially a district is empty
        District(int row, int col){
            this -> row = row;
            this -> col = col;
            front = nullptr;
            back = front;
            districtPopul = 0;//initial pop of 0
        }

        int getDistrPop(){
            return districtPopul;
        }

        //not necessary, good for improving utility
        int getRow(){
            return row;
        }

        //not necessary, good for improving utility
        int getCol(){
            return col;
        }

    };
    //list of dead people
    struct graveyardInventory{
        person* front;//Declaration for where the first vector index will point
        person* back;//Declaration for where the last vector index will point
        int districtPopul;//keep track of population

        //Initially a district is empty
        graveyardInventory(){
            front = nullptr;
            back = front;
            districtPopul = 0;//initial pop of dead
        }

        //not necessary, good for improving utility
        int getDistrPop(){
            return districtPopul;
        }
    };

    vector<District*> DistrictPointers; //will point to people when created or become nullptr at death
    //index == id

    vector<person*> idPointers; //will point to people when created or become nullptr at death
    graveyardInventory* deadFront;
    graveyardInventory* deadBack;
    int numAlive;
    int numDead;
    int newID = 0;


  public:
    /**
    * constructor:  initializes a "world" with nrows and
    *    ncols (nrows*ncols districtcs) in which all 
    *    districtricts are empty (a wasteland!).
    */
    GridWorld(unsigned nrows, unsigned ncols)   { //create world
        this -> nrows = nrows;
        this -> ncols = ncols;
        int row;
        int col;


        graveyardInventory* deadDistr = new graveyardInventory();
        deadBack = deadFront = deadDistr;

        //To avoid new allocation of vectors within 10000 items
        idPointers.reserve(10000);
        for(int i = 0; i <10000;i++){
            idPointers[i]=nullptr;
        }
        for( int i = 0; i <nrows; i++){
            for (int j = 0; j <ncols; j++) {

                District *tmp = new District(i, j);
                //add district to vector of pointers at 1D index
                DistrictPointers.push_back(tmp);
                numOfDistr++;
            }
        }
    }

    ~GridWorld() {
        // your destructor code here.
        //Destrow ppl and distrs
        for (int i = 0; i < (nrows * ncols); i++) {
            int row; //storing location of district
            int col;
            person *current = DistrictPointers[i]->front;
            District *curr = DistrictPointers[i];

            //point all the pointers to nullptr and delete people(dead and alive)
            //set params of the people to zero (reset)
            idPointers[i] = nullptr;
            while (current != NULL) {
                person *next = current->next;
                current->row = 0;
                current->col = 0;
                current->id = 0;
                current->living = false;
                current->prev = nullptr;
                current->next = nullptr;
                delete current;
                current = next;
            }

            //reset vars of all the districts
            //delete the districts from vector pointers
            curr->districtPopul = 0;
            curr->row = 0;
            curr->col = 0;
            DistrictPointers[i]->front = nullptr;
            DistrictPointers[i]->back = DistrictPointers[i]->front;
            idPointers[i] = nullptr;
            curr = nullptr;
            delete curr;

        }
        //make all pointers in the deadpool nullptrs
        //set deadpool vars to 0 (reset)
        person *fro = deadFront->front;
        person *bak = deadBack->back;
        while (fro != NULL) {
            person *itr = fro->next;
            fro->row = 0;
            fro->col = 0;
            fro->id = 0;
            fro->living = false;
            fro->prev = nullptr;
            fro->next = nullptr;
            delete fro;
            fro = itr;
        }

        //Set deadpool and distr pointers to nullptrs
        //delete deadpool
        //reset values of GW to zero
        deadFront->districtPopul = 0;
        deadBack = deadFront;
        deadFront->front = nullptr;
        deadBack->back = nullptr;
        delete deadFront->front;
        delete deadBack->back;
        deadFront = nullptr;
        deadBack = deadFront;
        delete deadFront;
        DistrictPointers.shrink_to_fit();
        idPointers.shrink_to_fit();
        numAlive = 0;
        numDead = 0;
        newID = 0;
        nrows = 0;
        ncols = 0;
    }



        /*
         * function: birth
         * description:  if row/col is valid, a new person is created
         *   with an ID according to rules in handout.  New person is
         *   placed in district (row, col)
         *
         * return:  indicates success/failure
         */
    bool birth(int row, int col, int &id){
        if(row < nrows && col < ncols){//if the location is at one of the existing districts

            //if no dead id's create a new person
            if (deadFront->front == nullptr){
                id = newID;

                person* newPerson = new person(row, col, id);
                newID++;//the next new ID will be the next index
                idPointers[id] = newPerson;

                //push to the back of the list (have the back of the list point to this new person
                District* district = DistrictPointers[row*nrows+col];

                //If empty district
                if (district->front == nullptr) {

                    district->front = district->back = newPerson;
                }
                //push to back of district
                else {

                    newPerson->prev = district->back;
                    district->back->next = newPerson;
                    district->back = newPerson;
                }
                //increase number of ppl within the district
                district->districtPopul++;
                //increase number of living ppl
                numAlive++;
                return true;

            }
            //ID exists
            else {
                //pop id off graveyardInventory
                int val = deadFront->front->id;
                id = val;
                person* tmp;

                tmp = deadFront->front;
                deadFront->front = deadFront->front->next;
                delete tmp;

                //create new person
                person* newPerson = new person(row, col, val);
                idPointers[val] = newPerson;//existing index
                //add to the end of the district
                District* district =  DistrictPointers[row*ncols+col];
                if (district->front == nullptr) {
                    district->front = district->back = newPerson;
                } else {
                    newPerson->prev = district->back;
                    district->back->next = newPerson;
                    district->back = newPerson;
                }
                //decrease number of dead ppl
                numDead--;

                //increase number of ppl within the district
                district->districtPopul++;
                //update deadpool
                deadFront->districtPopul--;
                //increase number of living ppl
                numAlive++;
                return true;
            }
        }
        else{//no resourceful region exists, human can't be born
          return false;
        }
    }

    /*
     * function: death 
     * description:  if given person is alive, person is killed and
     *   data structures updated to reflect this change.
     *
     * return:  indicates success/failure
     */
    bool death(int personID){

        //if there are no ids, the index id does not exist or if person is dead return false
        if ((numAlive+numDead) == 0 || personID >= (numAlive + numDead)|| !(idPointers[personID]->isAlive()) ){

            return false;
        }

        else {

            //point to the person from the vector point to ppl
            person *livingPerson = idPointers[personID];
            int row1 = livingPerson->getRow();
            int col1 = livingPerson->getCol();
            District *district1 =  DistrictPointers[row1*nrows+col1];
            //If one person in the district
            if(district1->front == livingPerson && livingPerson->next == nullptr){

                // make the next of moving person the front
                district1->front = nullptr;
                district1->back = district1->front;

            }//If in front of more ppl
            else if(district1->front == livingPerson ){
                // make the next of moving person the front
                district1->front = livingPerson->next;
                //make prev of the new front(next of moving person) null
                livingPerson->next->prev = nullptr;
                //make next of moving person null
                livingPerson->next = nullptr;
            }
            //if at the back
            else if(district1->back == livingPerson){

                //make back the prev
                district1->back = livingPerson->prev;

                //make the prev next a nullptr
                livingPerson->prev->next = nullptr;
                //make prev a nullptr
                livingPerson->prev = nullptr;
            }

            //else carefully remove from center
            else {

                //make prev next point to next
                livingPerson->prev->next = livingPerson->next;
                //make next prev point to prev
                livingPerson->next->prev = livingPerson->prev;
                //make prev nullptr
                livingPerson->prev = nullptr;
                //make next nullptr
                livingPerson->next = nullptr;
            }
            //reduce the population of the original district
            district1->districtPopul--;
            numAlive--;


            //push to back of graveyard
            if (deadFront->front == nullptr) {
                deadFront->front = deadFront->back = livingPerson;
            } else {
                livingPerson->prev = deadBack->back;//update moving person previous pointer
                deadBack->back->next = livingPerson;//update new previous next pointer to new person
                deadBack->back = livingPerson;//make the back
            }
            //increase number of ppl within the dead district
            deadFront->districtPopul++;
            numDead++;

            //living overall population doesn't change
            // update to status
            livingPerson->living = false;

            return true;
        }
    }

    /*
     * function: whereis
     * description:  if given person is alive, his/her current residence
     *   is reported via reference parameters row and col.
     *
     * return:  indicates success/failure
     */
    bool whereis(int id, int &row, int &col)const{
        if(id < (numAlive + numDead) && idPointers[id]->isAlive()){
            person *tmp = idPointers[id];
            row = tmp->getRow();
            col = tmp->getCol();
            return true;
        }
        else {
            return false;
        }
    }

    /*
     * function: move
     * description:  if given person is alive, and specified target-row
     *   and column are valid, person is moved to specified district and
     *   data structures updated accordingly.
     *
     * return:  indicates success/failure
     *
     * comment/note:  the specified person becomes the 'newest' member
     *   of target district (least seniority) --  see requirements of members().
     */
    bool move(int id, int targetRow, int targetCol){
     // the id does not exist or the district does not exist or the person with the id is dead
     if(id > (numAlive + numDead) ||  targetRow > nrows || targetCol > ncols || !(idPointers[id]->isAlive())) {
         return false;
     }
    //else
    else {
         //move them from their district
         person *movingPerson = idPointers[id];
         int row1 = movingPerson->getRow();
         int col1 = movingPerson->getCol();
         //define the districts
         District *district1 = DistrictPointers[row1*nrows+col1];//curr/old district (original district)
         District *district2 = DistrictPointers[targetRow*nrows+targetCol];//new district
         //take out of old district and connect nodes
         //if at the front,
        if(district1->front == movingPerson && movingPerson->next == nullptr){

            // make the next of moving person the front
            district1->front = nullptr;
            district1->back = district1->front;
        }
        else if(district1->front == movingPerson ){

            // make the next of moving person the front
            district1->front = movingPerson->next;
            //make prev of the new front(next of moving person) null
            movingPerson->next->prev = nullptr;
            //make next of moving person null
            movingPerson->next = nullptr;
        }

         //if at the back
         else if(district1->back == movingPerson){
             //make back the prev
             district1->back = movingPerson->prev;
             //make the prev next a nullptr
             movingPerson->prev->next = nullptr;
             //make prev a nullptr
             movingPerson->prev = nullptr;
         }

         //else carefully remove from center
         else {
             //make prev next point to next
             movingPerson->prev->next = movingPerson->next;
             //make next prev point to prev
             movingPerson->next->prev = movingPerson->prev;
             //make prev nullptr
             movingPerson->prev = nullptr;
             //make next nullptr
             movingPerson->next = nullptr;
         }
         //reduce the population of the original district
         district1->districtPopul--;
         movingPerson->updateLocation(targetRow,targetCol);
         //push to back of new district
         if (district2->front == nullptr) {
             district2->front = district2->back = movingPerson;
         } else {
             movingPerson->prev = district2->back;//update moving person previous pointer
             district2->back->next = movingPerson;//update new previous next pointer to new person
             district2->back = movingPerson;//make the back
         }
         //update person's location

         //increase number of ppl within the new district
         district2->districtPopul++;
         //living overall population doesn't change

         //return true
         //return true
         return true;
     }
    }

    //id and district exist
    std::vector<int> * members(int row, int col)const{
        vector<int> *tmp = new vector<int>;
        if ( row>nrows || col>ncols){


            return tmp;
        }
        else {
            int pop = DistrictPointers[row * ncols + col]->districtPopul;
            if (pop == 0) {


                return tmp;
            } else {
                int ID;
                int pop = DistrictPointers[row * ncols + col]->getDistrPop();
            
                tmp->reserve(pop);
                //int back =tmp->back();


                District *tempdist = DistrictPointers[row * ncols + col];
                person *tmpPers = tempdist->front;
                //while (tmpPers != NULL){//
                for (int i = 0; i < pop; i++) {//
                    ID = tmpPers->getID();
                    tmp->push_back(ID);
                    tmpPers = tmpPers->next;
                }

                return tmp;
            }
        }
    }

    /*
     * function: population
     * description:  returns the current (living) population of the world.
     */
    int population()const{

      return numAlive;
    }
    
    /*
     * function: population(int,int)
     * description:  returns the current (living) population of specified
     *   district.  If district does not exist, zero is returned
     */
    int population(int row, int col)const{
        District* district = DistrictPointers[row*ncols+col];
        return district->getDistrPop();
    }

    /*
     * function: num_rows
     * description:  returns number of rows in world
     */
    int num_rows()const {
      return nrows;
    }

    /*
     * function: num_cols
     * description:  returns number of columns in world
     */
    int num_cols()const {
      return ncols;
    }



};

#endif
