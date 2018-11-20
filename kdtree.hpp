#include <utility>
#include <algorithm>
#include <cmath>
#include <iostream>

using namespace std;
/**
*  This function finds which Point is smallest based on the specified
*  dimension and uses Point's operator < in case of a tie.
**/
template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
 const Point<Dim>& second, int curDim) const
{
 if(first[curDim] == second[curDim]){
   return first < second;
}
return first[curDim] < second[curDim];
}
/**
*  This function determines if the target is closer to the currentBest
*  point or to the new potential point.
**/
template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
 const Point<Dim>& currentBest,
 const Point<Dim>& potential) const
{
 double cdist = 0;
 double pdist = 0;

 for(int i = 0; i < Dim; i++){

   double square1 = target[i] - currentBest[i];
   double square2 = target[i] - potential[i];

   cdist += square1*square1;//pow(, 2);
   pdist += square2*square2;//pow(, 2);

 }
 if(sqrt(pdist) < sqrt(cdist)){
  return true;
}
 if(sqrt(pdist) == sqrt(cdist)){
   return potential < currentBest;
 }
 return false;

}
/**
*  This function groups elements below a certain index as less than the current
*  element and groups elements above as greater than the current element.
**/
template <int Dim>
int KDTree<Dim>::partition(int l, int r, int m, int dim){

 Point<Dim> val = elem[m];

 iter_swap(elem.begin() + m, elem.begin() + r);
 int storeIdx = l;

 for (int j = l; j <= r - 1; j++) {
   if (smallerDimVal(elem[j], val, dim)) {
     iter_swap(elem.begin() + storeIdx, elem.begin() + j);
     storeIdx++;
   }
}

 iter_swap(elem.begin() + r, elem.begin() + storeIdx);
 return storeIdx;
}
/**
*  Returns the k-th smallest element of the list from left to right
*  recursively calls quickselect to find desired position.
**/
template <int Dim>
Point<Dim> KDTree<Dim>::quickselect(int l, int r, int k, int dim ) {

 if(l == r){
  return elem[l];
}
 int idx = l + floor(rand() % (r-l + 1));
 idx = partition(l, r, idx, dim);

 if (k == idx){
   return elem[k];
}
 else if(k < idx){
  return quickselect(l, idx - 1, k, dim);
}
 else{
   return quickselect(idx + 1, r, k, dim);
 }

}
/**
*  This function recursively builds the left and right branches of the
*  tree and uses quickselect to choose children.
**/
template<int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::buildtree(KDTreeNode* subroot, int l, int r, int nextDim){

 if(l > r){
   return nullptr;
 }
 int mid = floor((l+r)/2);

 subroot = new KDTreeNode(quickselect(l, r, mid, nextDim));
 nextDim = (nextDim + 1) % Dim;

 subroot->left = buildtree(subroot->left, l, mid - 1, nextDim);
 subroot->right = buildtree(subroot->right, mid + 1, r, nextDim);

 return subroot;
}
/**
*  This function is a constructor for KDTree.
**/
template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{

 elem = newPoints;
 root = buildtree(root, 0, elem.size()-1, 0);
}
/**
*  This function creates a copy of a KDTree.
**/
template <int Dim>
typename KDTree<Dim>::KDTreeNode* KDTree<Dim>::_copy(KDTreeNode* subroot, KDTreeNode* rhs){
 if(rhs == nullptr){
   return;
 }

 subroot = new KDTreeNode(rhs->point);
 subroot->left = _copy(subroot->left, rhs->left);
 subroot->right = _copy(subroot->right, rhs->right);

}
/**
*  This function deallocates a KDTree from memory.
**/
template <int Dim>
void KDTree<Dim>::_destroy(KDTreeNode* subroot){
 if(subroot == nullptr){
  return;
}
 _destroy(subroot->left);
 _destroy(subroot->right);
 delete subroot;
 subroot = nullptr;
}
/**
*  This function is the copy constructor.
**/
template <int Dim>
KDTree<Dim>::KDTree(const KDTree<Dim>& other) {

 KDTreeNode* next = new KDTreeNode();
 _copy(next, other);
}
/**
*  This function is the = operator overload.
**/
template <int Dim>
const KDTree<Dim>& KDTree<Dim>::operator=(const KDTree<Dim>& rhs) {

 if(&rhs != this){
 _destroy(this->root);
 _copy(this->root, rhs);
 }

 return *this;
}
/**
*  This function is the KDTree destructor.
**/
template <int Dim>
KDTree<Dim>::~KDTree() {
 _destroy(this->root);
}
/**
*  This function searches and finds the smallest hyper-rectangle that has the target
*  and goes down the tree till it is at a leaf but also must check going up the tree
*  nust check the radius to ensure that the current target is the best.
**/
template <int Dim>
void KDTree<Dim>::nearestRecur(const Point<Dim>& query, Point<Dim> & nearest, KDTreeNode * curr, int dim) const {
  int kVal  =  Dim;
 if(curr == nullptr){ return; }

 bool isSmaller = smallerDimVal(query, curr->point, dim);
 if (!isSmaller) {
     nearestRecur(query, nearest, curr->right, (dim+1) % kVal);

     bool replace= shouldReplace(query, nearest, curr->point) || query == curr->point;
     if(replace == true){
       nearest = curr->point;
     }

     double c = 0;
     for(int i = 0; i < kVal; i++){
       double square = query[i] - nearest[i];
       c = c+ square*square;
     }

     bool isGreater  = sqrt(c) >= abs(curr->point[dim] - query[dim]);
     if(isGreater){
       nearestRecur(query, nearest, curr->left, (dim+1) % kVal);
     }

 }
 else{
   nearestRecur(query, nearest, curr->left, (dim+1) % Dim);

   bool needToReplace = (shouldReplace(query, nearest, curr->point) || query == curr->point);

   if(needToReplace){
     nearest = curr->point;
   }

   double c = 0;
   for(int i = 0; i < kVal; i++){
     double square = query[i] - nearest[i];
     c = c + square*square;
   }

   bool diff = sqrt(c) >= abs(curr->point[dim] - query[dim]);
   if(diff)
    nearestRecur(query, nearest, curr->right, (dim+1) % kVal);
  }


}
/**
*  This function finds the nearest neighbor to the given Point.
**/
template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
 Point<Dim> near = root->point;
 nearestRecur(query, near, root, 0);

 return near;
}
