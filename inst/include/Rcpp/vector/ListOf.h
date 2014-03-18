// ListOf.h: Rcpp R/C++ interface class library -- templated List container
//
// Copyright (C) 2014 Dirk Eddelbuettel, Romain Francois and Kevin Ushey
//
// This file is part of Rcpp.
//
// Rcpp is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Rcpp is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Rcpp.  If not, see <http://www.gnu.org/licenses/>.

#ifndef Rcpp_vector_ListOf_h_
#define Rcpp_vector_ListOf_h_

namespace Rcpp {

template <typename T>
class ListOf {

public:
    static const int RTYPE = traits::r_sexptype_traits<T>::rtype;
    typedef typename traits::r_vector_iterator<RTYPE>::type iterator ;
    typedef typename traits::r_vector_const_iterator<RTYPE>::type const_iterator ;

    ListOf(): list(R_NilValue) {};

    ListOf(SEXP data_): list(data_) {};

    template <typename U>
    ListOf(const U& data_): list(data_) {};

    ListOf(const ListOf& other): list(other.list) {};
    ListOf& operator=(const ListOf& other) {
        if (this != &other) {
            list = other.list;
        }
        return *this;
    }

    template <typename U>
    ListOf& operator=(const U& other) {
        list = as<List>(other);
        return *this;
    }

    // subsetting operators

    T operator[](int i) {
      return list[i];
    }

    const T operator[](int i) const {
      return list[i];
    }

    T operator[](const std::string& str) {
        return list[str];
    }

    const T operator[](const std::string& str) const {
        return list[str];
    }

    // iteration operators pass down to list

    inline iterator begin() {
        return list.begin();
    }

    inline iterator end() {
        return list.end();
    }

    inline const_iterator begin() const {
        return list.begin();
    }

    inline const_iterator end() const {
        return list.end();
    }

    class ListOfProxy {

    public:
        ListOfProxy(List& list_): list(list_) {};

        List::Proxy operator[](int i) {
            return List::Proxy(list, i);
        }

        List::Proxy operator[](const char* str) {
            std::vector<std::string> names = as< std::vector<std::string> >(list.attr("names"));
            for (int i=0; i < list.size(); ++i) {
                if (names[i] == str) {
                    return List::Proxy(list, i);
                }
            }
            std::stringstream ss;
            ss << "No name '" << str << "' in the names of the list supplied";
            stop(ss.str());
            return List::Proxy(list, -1);
        }

    private:
        ListOfProxy() {};

        List& list;
    };

    friend class ListOfProxy;

    // lhs access
    friend ListOfProxy lhs(ListOf& x) {
        return ListOfProxy(x.list);
    }

    // conversion operators
    operator SEXP() const { return wrap(list); }
    operator List() const { return list; }

    List list;

}; // ListOf<T>

// sapply, lapply wrappers

namespace sugar {

template <int RTYPE, bool NA, typename T, typename Function>
class Lapply;

template <int RTYPE, bool NA, typename T, typename Function, bool NO_CONVERSION>
class Sapply;

}

template <typename T, typename Function>
List lapply(const ListOf<T>& t, Function fun) {
    return lapply(t.list, fun);
}

template <typename T, typename Function>
T sapply(const ListOf<T>& t, Function fun) {
    return sapply(t.list, fun);
}

} // Rcpp

#endif