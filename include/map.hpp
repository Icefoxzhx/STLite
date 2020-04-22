/**
 * implement a container like std::map
 */
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

    template<class Key,class T,class Compare = std::less<Key> >
    class map {
    public:
        typedef pair<const Key, T> value_type;
        class const_iterator;
        class iterator;
    private:
        struct node{
            node *ls,*rs,*fa,*nxt,*pre;
            value_type *v;
            int sz;
            node(value_type *_v= nullptr,node *_f= nullptr,node *_l= nullptr,node *_r= nullptr,int _sz=1,node *_p= nullptr,node *_n= nullptr):
                v(_v),fa(_f),ls(_l),rs(_r),sz(_sz),pre(_p),nxt(_n){}
            node(const node &other):
                fa(other.fa),ls(other.ls),rs(other.rs),sz(other.sz),pre(other.pre),nxt(other.nxt){
                v=other.v== nullptr? nullptr:new value_type(*other.v);
            }
            void update(){
                sz=(ls== nullptr?0:ls->sz)+(rs== nullptr?0:rs->sz)+1;
            }
            ~node(){
                if(v!= nullptr) delete v;
            }
        }*rt,*tail;
        bool equal(const value_type *v,const Key &key) const {
            if(v== nullptr) return false;
            return !(Compare()(v->first,key)||Compare()(key,v->first));
        }
        bool cmp(const value_type *v,const Key &key) const {//true if less than
            if(v== nullptr) return false;
            return Compare()(v->first,key);
        }
        void LDR(node *p,node **ldrArr,int &ldrNum){
            if(p->ls) LDR(p->ls,ldrArr,ldrNum);
            ldrArr[ldrNum++]=p;
            if(p->rs) LDR(p->rs,ldrArr,ldrNum);
        }
        bool lr(node *p){
            return p==p->fa->ls;
        }
        void rotate(node *p){
            node *const fa=p->fa;
            if(fa==rt) rt=p;
            else (lr(fa)?fa->fa->ls:fa->fa->rs)=p;
            p->fa=fa->fa;
            if(p==fa->ls){
                if(p->rs) p->rs->fa=fa;
                fa->ls=p->rs;
                p->rs=fa;
                fa->fa=p;
            }else{
                if(p->ls) p->ls->fa=fa;
                fa->rs=p->ls;
                p->ls=fa;
                fa->fa=p;
            }
            fa->update();p->update();
        }
        void splay(node *p){
            node *pp=p;
            while(pp!= nullptr){
                pp->update();
                pp=pp->fa;
            }
            if(p==nullptr) return;
            while(p!=rt){
                if(p->fa!=rt){
                    if(lr(p)^lr(p->fa)) rotate(p);
                    else rotate(p->fa);
                }rotate(p);
            }
        }
        node *copyAll(node *p){
            node *q=new node(*p);
            if(p->ls) q->ls=copyAll(p->ls),q->ls->fa=q;
            if(p->rs) q->rs=copyAll(p->rs),q->rs->fa=q;
            return q;
        }
        void delAll(){
            node *p=tail;
            while(p!=nullptr){
            	node *pp=p;
            	p=p->pre;
            	delete pp;
            }
        }
        node *Find(const Key& key) const {
            node *p=rt;
            while(p!= nullptr){
                if(equal(p->v,key)) return p;
                if(cmp(p->v,key)) p=p->rs;
                else p=p->ls;
            }
            return nullptr;
        }
        node *findBegin() const {
            node *p=rt;
            while(p->ls!= nullptr) p=p->ls;
            return p;
        }
        pair<iterator,bool> ins(const value_type &v){
            node *p=rt;
            while(p!= nullptr){
                if(equal(p->v,v.first)){
                    splay(p);
                    return pair<iterator,bool>(iterator(this,p),false);
                }
                if(cmp(p->v,v.first)){
                    if(p->rs) p=p->rs;
                    else{
                        p->rs=new node(new value_type(v),p,nullptr,nullptr,1,p,p->nxt);
                        if(p->nxt!=nullptr) p->nxt->pre=p->rs;
                        p->nxt=p->rs;
                        p=p->rs;
                        break;
                    }
                }else{
                    if(p->ls) p=p->ls;
                    else{
                        p->ls=new node(new value_type(v),p,nullptr,nullptr,1,p->pre,p);
                        if(p->pre!=nullptr) p->pre->nxt=p->ls;
                        p->pre=p->ls;
                        p=p->ls;
                        break;
                    }
                }
            }
            splay(p);
            return pair<iterator,bool>(iterator(this,p),true);
        }
        void del(node *p){
            splay(p);
            if(p->ls== nullptr){
                rt=p->rs;
                p->rs->fa= nullptr;
                if(p->nxt) p->nxt->pre=p->pre;
                if(p->pre) p->pre->nxt=p->nxt;
                delete p;
                return;
            }
            if(p->rs== nullptr){
                rt=p->ls;
                p->ls->fa= nullptr;
                if(p->nxt) p->nxt->pre=p->pre;
                if(p->pre) p->pre->nxt=p->nxt;
                delete p;
                return;
            }
            node *q=p->pre;// pre
            splay(q);
            q->rs=p->rs;
            p->rs->fa=q;
            if(p->nxt) p->nxt->pre=p->pre;
            if(p->pre) p->pre->nxt=p->nxt;
            delete p;
            rt->update();
        }
    public:
        class iterator {
        public:
            node *cur;
            map *bel;
            iterator(map *_b= nullptr,node *_c= nullptr):bel(_b),cur(_c) {}
            iterator(const iterator &other) {
                bel=other.bel;
                cur=other.cur;
            }
            iterator operator++(int) {
                auto tmp=*this;
                if(cur->nxt== nullptr) throw invalid_iterator();
				cur=cur->nxt;
                return tmp;
            }
            iterator & operator++() {
                if(cur->nxt== nullptr) throw invalid_iterator();
				cur=cur->nxt;
                return *this;
            }
            iterator operator--(int) {
                auto tmp=*this;
                if(cur->pre== nullptr) throw invalid_iterator();
				cur=cur->pre;
                return tmp;
            }
            iterator & operator--() {
                if(cur->pre== nullptr) throw invalid_iterator();
				cur=cur->pre;
                return *this;
            }
            value_type & operator*() const {
                return *(cur->v);
            }
            bool operator==(const iterator &rhs) const {
                return bel==rhs.bel&&cur==rhs.cur;
            }
            bool operator==(const const_iterator &rhs) const {
                return bel==rhs.bel&&cur==rhs.cur;
            }
            bool operator!=(const iterator &rhs) const {
                return !(*this==rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this==rhs);
            }
            value_type* operator->() const noexcept {
                return cur->v;
            }
        };
        class const_iterator {
        public:
            const node *cur;
            const map *bel;
            const_iterator(const map *_b= nullptr,const node *_c= nullptr):bel(_b),cur(_c) {}
            const_iterator(const iterator &other) {
                bel=other.bel;
                cur=other.cur;
            }
            const_iterator(const const_iterator &other) {
                bel=other.bel;
                cur=other.cur;
            }
			const_iterator operator++(int) {
				auto tmp=*this;
				if(cur->nxt== nullptr) throw invalid_iterator();
				cur=cur->nxt;
				return tmp;
			}
			const_iterator & operator++() {
				if(cur->nxt== nullptr) throw invalid_iterator();
				cur=cur->nxt;
				return *this;
			}
			const_iterator operator--(int) {
				auto tmp=*this;
				if(cur->pre== nullptr) throw invalid_iterator();
				cur=cur->pre;
				return tmp;
			}
			const_iterator & operator--() {
				if(cur->pre== nullptr) throw invalid_iterator();
				cur=cur->pre;
				return *this;
			}
            const value_type & operator*() const {
                return *(cur->v);
            }
            bool operator==(const iterator &rhs) const {
                return bel==rhs.bel&&cur==rhs.cur;
            }
            bool operator==(const const_iterator &rhs) const {
                return bel==rhs.bel&&cur==rhs.cur;
            }
            bool operator!=(const iterator &rhs) const {
                return !(*this==rhs);
            }
            bool operator!=(const const_iterator &rhs) const {
                return !(*this==rhs);
            }
            const value_type* operator->() const noexcept {
                return cur->v;
            }
        };
        map() {tail=rt=new node();}
        map(const map &other) {
            rt=copyAll(other.rt);
            int sz=rt->sz;
            if(sz==1){tail=rt;return;}
            node **nodes=new node*[sz];
            int LDRnum=0;
            LDR(rt,nodes,LDRnum);
            for(int i=0;i<sz-1;++i){
                nodes[i]->nxt=nodes[i+1];
                nodes[i+1]->pre=nodes[i];
            }
            tail=nodes[sz-1];
            delete []nodes;
        }
        map & operator=(const map &other) {
            if(this==&other) return *this;
            delAll();
            tail=rt=copyAll(other.rt);
            int sz=rt->sz;
            if(sz==1){tail=rt;return *this;}
            node **nodes=new node*[sz];
            int LDRnum=0;
            LDR(rt,nodes,LDRnum);
            for(int i=0;i<sz-1;++i){
                nodes[i]->nxt=nodes[i+1];
                nodes[i+1]->pre=nodes[i];
            }
            tail=nodes[sz-1];
            delete []nodes;
            return *this;
        }
        ~map() {
            delAll();
        }
        /**
         * TODO
         * access specified element with bounds checking
         * Returns a reference to the mapped value of the element with key equivalent to key.
         * If no such element exists, an exception of type `index_out_of_bound'
         */
        T & at(const Key &key) {
            node *ans=Find(key);
            if(ans== nullptr) throw index_out_of_bound();
            return ans->v->second;
        }
        const T & at(const Key &key) const {
            node *ans=Find(key);
            if(ans== nullptr) throw index_out_of_bound();
            return ans->v->second;
        }
        /**
         * TODO
         * access specified element
         * Returns a reference to the value that is mapped to a key equivalent to key,
         *   performing an insertion if such key does not already exist.
         */
        T & operator[](const Key &key) {
            return insert(value_type(key,T())).first.cur->v->second;
        }
        /**
         * behave like at() throw index_out_of_bound if such key does not exist.
         */
        const T & operator[](const Key &key) const {
            return at(key);
        }
        iterator begin() {
            return iterator(this,findBegin());
        }
        const_iterator cbegin() const {
            return const_iterator(this,findBegin());
        }
        iterator end() {
            return iterator(this,tail);
        }
        const_iterator cend() const {
            return const_iterator(this,tail);
        }
        bool empty() const {
            return size()==0;
        }
        size_t size() const {
            return rt->sz-1;
        }
        void clear() {
            delAll();tail=rt=new node();
        }
        /**
         * insert an element.
         * return a pair, the first of the pair is
         *   the iterator to the new element (or the element that prevented the insertion),
         *   the second one is true if insert successfully, or false.
         */
        pair<iterator, bool> insert(const value_type &value) {
            return ins(value);
        }
        /**
         * erase the element at pos.
         *
         * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
         */
        void erase(iterator pos) {
            if(pos.bel!=this||pos.cur->v== nullptr) throw invalid_iterator();
            del(pos.cur);
        }
        /**
         * Returns the number of elements with key
         *   that compares equivalent to the specified argument,
         *   which is either 1 or 0
         *     since this container does not allow duplicates.
         * The default method of check the equivalence is !(a < b || b > a)
         */
        size_t count(const Key &key) const {
            node *ans=Find(key);
            if(ans== nullptr) return 0;
            else return 1;
        }
        /**
         * Finds an element with key equivalent to key.
         * key value of the element to search for.
         * Iterator to an element with key equivalent to key.
         *   If no such element is found, past-the-end (see end()) iterator is returned.
         */
        iterator find(const Key &key) {
            node *ans=Find(key);
            if(ans== nullptr) return end();
            return iterator(this,ans);
        }
        const_iterator find(const Key &key) const {
            node *ans=Find(key);
            if(ans== nullptr) return cend();
            return const_iterator(this,ans);
        }
    };

}

#endif