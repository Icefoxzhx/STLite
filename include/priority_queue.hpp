#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"
using namespace std;
namespace sjtu {


template<typename T, class Compare = std::less<T> >
class priority_queue {
private:
    struct node{
        node *ls,*rs;
        T key;
        int dis;
        explicit node(const T &_key):key(_key), ls(nullptr), rs(nullptr), dis(0){}
    }*root;
    node *merge(node *p1,node *p2){
        if(p1==nullptr) return p2;
        if(p2==nullptr) return p1;
        if(Compare()(p1->key,p2->key)) swap(p1,p2);
        p1->rs=merge(p1->rs,p2);
        if(p1->ls==nullptr || (p1->rs!=nullptr&&p1->rs->dis > p1->ls->dis)) swap(p1->rs,p1->ls);
        p1->dis=(p1->rs==nullptr?-1:p1->rs->dis)+1;
        return p1;
    }
    node *copy(node *p){
        if(p==nullptr) return nullptr;
        node *q=new node(p->key);
        q->ls=copy(p->ls);
        q->rs=copy(p->rs);
        q->dis=p->dis;
        return q;
    }
    void del(node *p){
        if(p== nullptr) return;
        del(p->ls);
        del(p->rs);
        delete p;
    }
    size_t sz;
public:

	priority_queue():root(nullptr),sz(0){}
	priority_queue(const priority_queue &other) {
	    root=copy(other.root);
	    sz=other.sz;
	}

	~priority_queue() {
	    del(root);
	}

	priority_queue &operator=(const priority_queue &other) {
	    if(this==&other) return *this;
	    del(root);
	    root=copy(other.root);
	    sz=other.sz;
	    return *this;
	}

	const T & top() const {
        if(sz==0) throw container_is_empty();
        return root->key;
	}

	void push(const T &e) {
        node *p=new node(e);
        root=merge(root,p);
        ++sz;
	}

	void pop() {
        if(sz==0) throw container_is_empty();
        node *p=root;
        root=merge(root->ls,root->rs);
        delete p;
        --sz;
	}

	size_t size() const {
        return sz;
	}

	bool empty() const {
        return sz==0;
	}

	void merge(priority_queue &other) {
	    root=merge(root,other.root);
	    sz+=other.sz;
	    other.root= nullptr;
	    other.sz=0;
	}
};

}

#endif
