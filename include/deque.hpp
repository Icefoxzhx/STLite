#ifndef SJTU_DEQUE_HPP
#define SJTU_DEQUE_HPP
#include "exceptions.hpp"

#include <cstddef>

namespace sjtu { 

template<class T>
class deque {
private:
    static int const BN=512;
    static int const MERGE_N=BN*0.5;
    struct node{
        node *nxt,*pre;
        T *x;
        node(node *_n= nullptr,node *_p= nullptr):x(nullptr),nxt(_n),pre(_p){}
        node(T _x,node *_n= nullptr,node *_p= nullptr){
            x=new T(_x);
            nxt=_n;
            pre=_p;
        }
        node(const node &&other){
            nxt=other.nxt;
            pre=other.pre;
            x=other.x;
            other.x= nullptr;
        }
        node(const node &other){
            nxt=other.nxt;
            pre=other.pre;
            x=other.x== nullptr?nullptr:new T(*other.x);
        }
        node &operator=(const node &other){
            if(&other==this) return *this;
            if(x!=nullptr) delete x;
            nxt=other.nxt;
            pre=other.pre;
            x=other.x== nullptr?nullptr:new T(*other.x);
        }
        node &operator=(const node &&other){
            if(&other==this) return *this;
            if(x!=nullptr) delete x;
            nxt=other.nxt;
            pre=other.pre;
            x=other.x;
            other.x= nullptr;
        }
        ~node(){
            if(x!=nullptr) delete x;
        }
        friend void movenxt(const node *&p,int n){
            while(n--) p=p->nxt;
        }
        friend void movenxt(node *&p,int n){
            while(n--) p=p->nxt;
        }
    };
    struct Bnode{
        node *head;
        Bnode *nxt,*pre;
        int sz;
        Bnode(node *p= nullptr,Bnode *_n= nullptr,Bnode *_p= nullptr,int _sz=0,node *_t= nullptr):head(p),nxt(_n),pre(_p),sz(_sz){}
        ~Bnode(){
            if(sz){
                node *p=head->nxt;
                while(p!=head){
                    node *tmp=p;
                    p=p->nxt;
                    delete tmp;
                }
            }
            delete head;
        }
        friend void merge(Bnode *p1,Bnode *p2){
            node *q1=p1->head->pre;
            q1->nxt=p2->head->nxt;
            q1->nxt->pre=q1;
            p1->head->pre=p2->head->pre;
            p2->head->pre->nxt=p1->head;
            p2->head->nxt= nullptr;
            p1->sz+=p2->sz;
            p2->sz=0;
            p1->nxt=p2->nxt;
            p2->nxt->pre=p1;
            delete p2;
        }
        void split(){
            nxt=new Bnode(new node,nxt,this,sz-sz/2);
            nxt->nxt->pre=nxt;
            sz=sz/2;
            node *p=head;
            movenxt(p,sz);
            nxt->head->nxt=p->nxt;
            p->nxt->pre=nxt->head;
            nxt->head->pre=head->pre;
            head->pre->nxt=nxt->head;
            head->pre=p;
            p->nxt=head;
        }
        void trymerge(){
            if(!sz){
                pre->nxt=nxt;
                nxt->pre=pre;
                delete this;
                return;
            }
            if(nxt->sz!=0&&sz+nxt->sz<MERGE_N){
                merge(this,nxt);
            }else if(pre->sz!=0&&sz+pre->sz<MERGE_N){
                merge(pre,this);
            }
        }
        void insert(node *pp,const T &value){//insert before pp
            if(!sz){
                head->nxt=new node(value,head,head);
                head->pre=head->nxt;
                ++sz;return;
            }
            node *p=head;
            while(p->nxt!=pp) p=p->nxt;
            p->nxt=new node(value,p->nxt,p);
            p=p->nxt;
            p->nxt->pre=p;
            ++sz;
            if(sz==BN) split();
        }
        void erase(node *pp){
            node *p=head;
            while(p->nxt!=pp) p=p->nxt;
            p->nxt=pp->nxt;
            if(pp->nxt!=nullptr) pp->nxt->pre=p;
            delete pp;
            --sz;
            trymerge();
        }
        T &findkth(int n){
            Bnode *p=this;
            while(p->sz<n){
                n-=p->sz;
                p=p->nxt;
            }
            node *q=p->head;
            while(n--) q=q->nxt;
            return *(q->x);
        }
    };
    friend void movenxt(const Bnode *&Bp,const node *&cur,int n){
        while(Bp->sz&&n>0&&cur->nxt!=Bp->head){
            --n;
            cur=cur->nxt;
        }
        while(n>0){
            Bp=Bp->nxt;
            //if(Bp==nullptr) throw invalid_iterator();
            if(n<=Bp->sz){
                cur=Bp->head;
                movenxt(cur,n);
                break;
            }
            n-=Bp->sz;
        }
    }
    friend void movenxt(Bnode *&Bp,node *&cur,int n){
        while(Bp->sz&&n>0&&cur->nxt!=Bp->head){
            --n;
            cur=cur->nxt;
        }
        while(n>0){
            Bp=Bp->nxt;
            //if(Bp==nullptr) throw invalid_iterator();
            if(n<=Bp->sz){
                cur=Bp->head;
                movenxt(cur,n);
                break;
            }
            n-=Bp->sz;
        }
    }
    friend void movepre(const Bnode *&Bp,const node *&cur,int n){
        while(Bp->sz&&n>0&&cur->pre!=Bp->head){
            --n;
            cur=cur->pre;
        }
        while(n>0){
            Bp=Bp->pre;
            if(n<=Bp->sz){
                cur=Bp->head;
                while(n--) cur=cur->pre;
                break;
            }
            n-=Bp->sz;
        }
    }
    friend void movepre(Bnode *&Bp,node *&cur,int n){
        while(Bp->sz&&n>0&&cur->pre!=Bp->head){
            --n;
            cur=cur->pre;
        }
        while(n>0){
            Bp=Bp->pre;
            if(n<=Bp->sz){
                cur=Bp->head;
                while(n--) cur=cur->pre;
                break;
            }
            n-=Bp->sz;
        }
    }
public:
	class const_iterator;
	class iterator {
    public:
		Bnode *Bp;
		node *cur;
        deque *bel;
        int id;
	    iterator(Bnode *_Bp= nullptr,node *_cur= nullptr,deque *_bel= nullptr,int _id=0):Bp(_Bp),cur(_cur),bel(_bel),id(_id){}
		/**
		 * return a new iterator which pointer n-next elements
		  *   if there are not enough elements, iterator becomes invalid
		 * as well as operator-
		 */
        iterator& operator+=(const int &n) {
            id+=n;
            if(id<0||id>bel->sz) return *this;// ub
            if(id==bel->sz){Bp=bel->rt,cur=bel->rt->head;return *this;}//end()
            n>0?movenxt(Bp,cur,n):movepre(Bp,cur,-n);
            return *this;
        }
        iterator& operator-=(const int &n) {
            id-=n;
            if(id<0||id>bel->sz) return *this;// ub
            if(id==bel->sz){Bp=bel->rt,cur=bel->rt->head;return *this;}//end()
            n>0?movepre(Bp,cur,n):movenxt(Bp,cur,-n);
            return *this;
        }
		iterator operator+(const int &n) const {
		    auto res=*this;
		    res+=n;
			return res;
		}
		iterator operator-(const int &n) const {
            auto res=*this;
            res-=n;
            return res;
		}
		// return the distance between two iterator,
		// if these two iterators points to different deques, throw invaild_iterator.
		int operator-(const iterator &rhs) const {
			if(bel!=rhs.bel) throw invalid_iterator();
            return id-rhs.id;
		}
		/**
		 * TODO iter++
		 */
		iterator operator++(int) {
		    auto tmp=*this;
		    *this+=1;
		    return tmp;
		}
		/**
		 * TODO ++iter
		 * 		throw if iterator is invalid
		 */
		iterator& operator++() {
		    *this+=1;return *this;
		}
		/**
		 * TODO iter--
		 * 		throw if iterator is invalid
		 */
		iterator operator--(int) {
            auto tmp=*this;
            *this-=1;
            return tmp;
		}
		/**
		 * TODO --iter
		 */
		iterator& operator--() {
            *this-=1;return *this;
		}
		/**
		 * TODO *it
		 */
		T& operator*() const {
		    if(id<0||id>=bel->sz) throw invalid_iterator();
		    return *(cur->x);
		}
		/**
		 * TODO it->field
		 */
		T* operator->() const {
            if(id<0||id>=bel->sz) throw invalid_iterator();
		    return cur->x;
		}
		/**
		 * a operator to check whether two iterators are same (pointing to the same memory).
		 */
		bool operator==(const iterator &rhs) const {
		    return cur==rhs.cur&&id==rhs.id&&bel==rhs.bel;
		}
		bool operator==(const const_iterator &rhs) const {
		    return cur==rhs.cur&&id==rhs.id&&bel==rhs.bel;
		}
		/**
		 * some other operator for iterator.
		 */
		bool operator!=(const iterator &rhs) const {
		    return !(*this==rhs);
		}
		bool operator!=(const const_iterator &rhs) const {
		    return !(*this==rhs);
		}
	};
	class const_iterator {
		// it should has similar member method as iterator.
		//  and it should be able to construct from an iterator.
	private:
        const Bnode *Bp;
        const node *cur;
        const deque *bel;
        int id;
	public:
        const_iterator(const Bnode *_Bp= nullptr,const node *_cur= nullptr,const deque *_bel= nullptr,int _id=0):Bp(_Bp),cur(_cur),bel(_bel),id(_id){}
        const_iterator(const const_iterator &other):Bp(other.Bp),cur(other.cur),bel(other.bel),id(other.id) {}
        const_iterator(const iterator &other):Bp(other.Bp),cur(other.cur),bel(other.bel),id(other.id) {}
        const_iterator& operator+=(const int &n) {
            id+=n;
            if(id<0||id>bel->sz) return *this;// ub
            if(id==bel->sz){Bp=bel->rt,cur=bel->rt->head;return *this;}//end()
            n>0?movenxt(Bp,cur,n):movepre(Bp,cur,-n);
            return *this;
        }
        const_iterator& operator-=(const int &n) {
            id-=n;
            if(id<0||id>bel->sz) return *this;// ub
            if(id==bel->sz){Bp=bel->rt,cur=bel->rt->head;return *this;}//end()
            n>0?movepre(Bp,cur,n):movenxt(Bp,cur,-n);
            return *this;
        }
        const_iterator operator+(const int &n) const {
            auto res=*this;
            res+=n;
            return res;
        }
        const_iterator operator-(const int &n) const {
            auto res=*this;
            res-=n;
            return res;
        }
        // return the distance between two iterator,
        // if these two iterators points to different deques, throw invaild_iterator.
        int operator-(const const_iterator &rhs) const {
            if(bel!=rhs.bel) throw invalid_iterator();
            return id-rhs.id;
        }
        const_iterator operator++(int) {
            auto tmp=*this;
            *this+=1;
            return tmp;
        }
        const_iterator& operator++() {
            *this+=1;return *this;
        }
        const_iterator operator--(int) {
            auto tmp=*this;
            *this-=1;
            return tmp;
        }
        const_iterator& operator--() {
            *this-=1;return *this;
        }
        const T& operator*() const {
            if(id<0||id>=bel->sz) throw invalid_iterator();
            return *(cur->x);
        }
        /**
         * TODO it->field
         */
        const T* operator->() const {
            if(id<0||id>=bel->sz) throw invalid_iterator();
            return cur->x;
        }
        bool operator==(const iterator &rhs) const {
            return cur==rhs.cur&&id==rhs.id&&bel==rhs.bel;
        }
        bool operator==(const const_iterator &rhs) const {
            return cur==rhs.cur&&id==rhs.id&&bel==rhs.bel;
        }
        bool operator!=(const iterator &rhs) const {
            return !(*this==rhs);
        }
        bool operator!=(const const_iterator &rhs) const {
            return !(*this==rhs);
        }
	};
	/**
	 * TODO Constructors
	 */
	Bnode *rt;
	int sz;
	deque() {
	    rt=new Bnode(new node);
	    sz=0;
	}
	deque(const deque &other) {
        sz=other.sz;
        rt=new Bnode(new node);
        Bnode *p1=rt,*p2=other.rt;
        while(p2->nxt!= nullptr&&p2->nxt!=other.rt){
            p2=p2->nxt;
            p1->nxt=new Bnode(new node, nullptr,p1,p2->sz);
            p1=p1->nxt;
            if(p2->sz==0) continue;
            node *q1=p1->head,*q2=p2->head;
            while(q2->nxt!=p2->head){
                q2=q2->nxt;
                q1->nxt=new node(*(q2->x), nullptr,q1);
                q1=q1->nxt;
            }
            q1->nxt=p1->head;
            p1->head->pre=q1;
        }
        rt->pre=p1;
        p1->nxt=rt;
	}
	~deque() {
	    clear();
	    delete rt;
	}
	deque &operator=(const deque &other) {
	    if(this==&other) return *this;
	    clear();
        Bnode *p1=rt,*p2=other.rt;
        sz=other.sz;
        while(p2->nxt!=nullptr&&p2->nxt!=other.rt){
            p2=p2->nxt;
            p1->nxt=new Bnode(new node, nullptr,p1,p2->sz);
            p1=p1->nxt;
            if(p2->sz==0) continue;
            node *q1=p1->head,*q2=p2->head;
            while(q2->nxt!=p2->head){
                q2=q2->nxt;
                q1->nxt=new node(*(q2->x),nullptr,q1);
                q1=q1->nxt;
            }
            q1->nxt=p1->head;
            p1->head->pre=q1;
        }
        rt->pre=p1;
        p1->nxt=rt;
        return *this;
	}
	/**
	 * access specified element with bounds checking
	 * throw index_out_of_bound if out of bound.
	 */
	T & at(const size_t &pos) {
	    if(pos>=sz||pos<0) throw index_out_of_bound();
	    return rt->nxt->findkth(pos+1);
	}
	const T & at(const size_t &pos) const {
        if(pos>=sz||pos<0) throw index_out_of_bound();
        return rt->nxt->findkth(pos+1);
	}
	T & operator[](const size_t &pos) {
        if(pos>=sz||pos<0) throw index_out_of_bound();
        return rt->nxt->findkth(pos+1);
	}
	const T & operator[](const size_t &pos) const {
        if(pos>=sz||pos<0) throw index_out_of_bound();
        return rt->nxt->findkth(pos+1);
	}
	/**
	 * access the first element
	 * throw container_is_empty when the container is empty.
	 */
	const T & front() const {
	    if(empty()) throw container_is_empty();
	    return rt->nxt->findkth(1);
	}
	/**
	 * access the last element
	 * throw container_is_empty when the container is empty.
	 */
	const T & back() const {
        if(empty()) throw container_is_empty();
        return rt->nxt->findkth(sz);
	}
	/**
	 * returns an iterator to the beginning.
	 */
	iterator begin() {
	    if(empty()) return iterator(rt,rt->head,this,0);
	    return iterator(rt->nxt,rt->nxt->head->nxt,this,0);
	}
	const_iterator cbegin() const {
        if(empty()) return const_iterator(rt,rt->head,this,0);
        return const_iterator(rt->nxt,rt->nxt->head->nxt,this,0);
	}
	/**
	 * returns an iterator to the end.
	 */
	iterator end() {
        return iterator(rt,rt->head,this,sz);
	}
	const_iterator cend() const {
        return const_iterator(rt,rt->head,this,sz);
	}
	/**
	 * checks whether the container is empty.
	 */
	bool empty() const {
	    return sz==0;
	}
	/**
	 * returns the number of elements
	 */
	size_t size() const {
	    return sz;
	}
	/**
	 * clears the contents
	 */
	void clear() {
	    if(empty()) return;
	    Bnode *p=rt->nxt;
	    while(p!= nullptr&&p!=rt){
	        Bnode *tmp=p;
	        p=p->nxt;
	        delete tmp;
	    }
	    rt->nxt=rt->pre= nullptr;
	    sz=0;
	}
	/**
	 * inserts elements at the specified locat on in the container.
	 * inserts value before pos
	 * returns an iterator pointing to the inserted value
	 *     throw if the iterator is invalid or it point to a wrong place.
	 */
	iterator insert(iterator pos, const T &value) {
	    if(pos.bel!=this) throw invalid_iterator();
	    if(pos.id==0){push_front(value);return begin();}
	    if(pos.id==sz){push_back(value);return --end();}
	    if(pos.id<0||pos.id>sz) throw invalid_iterator();
        int x=pos.id;
        pos.Bp->insert(pos.cur,value);
        ++sz;
        return begin()+x;
	}
	/**
	 * removes specified element at pos.
	 * removes the element at pos.
	 * returns an iterator pointing to the following element, if pos pointing to the last element, end() will be returned.
	 * throw if the container is empty, the iterator is invalid or it points to a wrong place.
	 */
	iterator erase(iterator pos) {
        if(empty()) throw container_is_empty();
        if(pos.bel!=this) throw invalid_iterator();
        if(pos.id==0){pop_front();return begin();}
        if(pos.id==sz-1){pop_back();return end();}
        if(pos.id<0||pos.id>=sz) throw invalid_iterator();
        int x=pos.id;
        pos.Bp->erase(pos.cur);
        --sz;
        return begin()+x;
	}
	/**
	 * adds an element to the end
	 */
	void push_back(const T &value) {
	    if(empty()){
	        rt->nxt=rt->pre=new Bnode(new node,rt,rt);
	    }
        ++sz;
	    Bnode *p=rt->pre;
        if(!p->sz){
            p->head->nxt=new node(value,p->head,p->head);
            p->head->pre=p->head->nxt;
            ++p->sz;return;
        }
        node *q=p->head->pre;
        q->nxt=new node(value,q->nxt,q);
        q=q->nxt;
        q->nxt->pre=q;
        ++p->sz;
        if(p->sz==BN) p->split();
	}
	/**
	 * removes the last element
	 *     throw when the container is empty.
	 */
	void pop_back() {
	    if(empty()) throw container_is_empty();
	    --sz;
        Bnode *p=rt->pre;
        node *qq=p->head->pre,*q=qq->pre;
        q->nxt=qq->nxt;
        if(qq->nxt!=nullptr) qq->nxt->pre=q;
        delete qq;
        --p->sz;
        p->trymerge();
	}
	/**
	 * inserts an element to the beginning.
	 */
	void push_front(const T &value) {
        if(empty()){
            rt->nxt=rt->pre=new Bnode(new node,rt,rt);
        }
        ++sz;
        Bnode *p=rt->nxt;
        if(!p->sz){
            p->head->nxt=new node(value,p->head,p->head);
            p->head->pre=p->head->nxt;
            ++p->sz;return;
        }
        node *q=p->head;
        q->nxt=new node(value,q->nxt,q);
        q=q->nxt;
        q->nxt->pre=q;
        ++p->sz;
        if(p->sz==BN) p->split();
	}
	/**
	 * removes the first element.
	 *     throw when the container is empty.
	 */
	void pop_front() {
        if(empty()) throw container_is_empty();
        --sz;
        Bnode *p=rt->nxt;
        node *qq=p->head->nxt,*q=qq->pre;
        q->nxt=qq->nxt;
        if(qq->nxt!=nullptr) qq->nxt->pre=q;
        delete qq;
        --p->sz;
        p->trymerge();
	}
};

}

#endif
