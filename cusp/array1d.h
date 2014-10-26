/*
 *  Copyright 2008-2014 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/**
 * \file
 * \brief 1D array of elements that may reside in "host" or "device" memory space
 */

#pragma once

#include <typeinfo>
#include <cusp/detail/config.h>

#include <cusp/memory.h>
#include <cusp/format.h>
#include <cusp/exception.h>

#include <thrust/copy.h>
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/detail/vector_base.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/iterator/counting_iterator.h>

namespace cusp
{

// forward declaration of array1d_view
template <typename Iterator> class array1d_view;

/**
 *  \addtogroup arrays Arrays
 *  \par Overview
 *  Basic 1D and 2D datatypes for processing algorithms
 *  in "host" and "device" memory space. Consists of containers,
 *  representing arrays with explicit memory allocations, and
 *  views, representing generic iterators wrapped in containers.
 */

/**
 *  \addtogroup array_containers Array Containers
 *  \ingroup arrays
 *  \{
 */

/**
 * \brief The array1d class is a 1D vector container that may contain elements
 * stored in "host" or "device" memory space
 *
 * \tparam T value_type of the array
 * \tparam MemorySpace memory space of the array (cusp::host_memory or cusp::device_memory)
 *
 * \par Overview
 * A array1d vector is a container that supports random access to elements.
 * The memory associated with a array1d vector may reside in either "host"
 * or "device" memory depending on the supplied allocator embedded in the
 * MemorySpace template argument. array1d vectors inherit a considerable
 * amount of functionality from the thrust::detail::vector_base case.
 *  \see http://thrust.github.io/doc/classthrust_1_1host__vector.html
 *  \see http://thrust.github.io/doc/classthrust_1_1device__vector.html
 *
 * \par Example
 * \code
 * // include cusp array1d header file
 * #include <cusp/array1d.h>
 *
 * int main()
 * {
 *   // Allocate a array of size 2 in "host" memory
 *   cusp::array1d<int,cusp::host_memory> a(2);
 *
 *   // Set the first element to 0 and second element to 1
 *   a[0] = 0;
 *   a[1] = 1;
 *
 *   // Allocate a seceond array in "device" memory that is
 *   // a copy of the first
 *   cusp::array1d<int,cusp::device_memory> b(a);
 * }
 * \endcode
 */
template <typename T, typename MemorySpace>
class array1d : public thrust::detail::vector_base<T, typename cusp::default_memory_allocator<T, MemorySpace>::type>
{
private:
    typedef typename cusp::default_memory_allocator<T, MemorySpace>::type Alloc;
    typedef typename thrust::detail::vector_base<T,Alloc>                 Parent;

public:
    /*! \cond */
    typedef MemorySpace                                 memory_space;
    typedef cusp::array1d_format                        format;

    typedef typename Parent::iterator                   iterator;
    typedef typename Parent::const_iterator             const_iterator;
    typedef typename Parent::value_type                 value_type;
    typedef typename Parent::size_type                  size_type;

    typedef cusp::array1d<T,MemorySpace>                container;
    typedef typename cusp::array1d_view<iterator>       view;
    typedef typename cusp::array1d_view<const_iterator> const_view;

    template<typename MemorySpace2>
    struct rebind {
        typedef cusp::array1d<T, MemorySpace2> type;
    };
    /*! \endcond */

    /*! This constructor creates an empty \p array1d vector.
     */
    array1d(void)
      : Parent() {}

    /*! This constructor creates a \p array1d vector with the given
     *  size.
     *  \param n The number of elements to initially create.
     */
    explicit array1d(size_type n)
        : Parent()
    {
        if(n > 0)
        {
            Parent::m_storage.allocate(n);
            Parent::m_size = n;
        }
    }

    /*! This constructor creates a \p array1d vector with copies
     *  of an exemplar element.
     *  \param n The number of elements to initially create.
     *  \param value An element to copy.
     */
    explicit array1d(size_type n, const value_type &value)
        : Parent(n, value) {}

    /*! Copy constructor copies from an exemplar array1d
     *  \param v The vector to copy.
     */
    array1d(const array1d &v)
        : Parent(v) {}

    /*! Assign operator copies from an exemplar \p array1d.
     *  \param v The \p array1d to copy.
     */
    array1d &operator=(const array1d &v)
    { Parent::operator=(v); return *this; }

    /*! Copy constructor copies from an exemplar std::vector with different type
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The array1d to copy.
     */
    template<typename OtherT, typename OtherMem>
    array1d(const array1d<OtherT, OtherMem> &v)
        : Parent(v) {}

    /*! Assign operator copies from an exemplar \p std::vector.
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The \p std::vector to copy.
     */
    template<typename OtherT, typename OtherMem>
    array1d &operator=(const array1d<OtherT, OtherMem> &v)
    { Parent::operator=(v); return *this; }

    /*! Copy constructor copies from an exemplar std::vector with different type
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The array1d to copy.
     */
    template<typename OtherT, typename OtherAlloc>
    array1d(const std::vector<OtherT, OtherAlloc> &v)
        : Parent(v) {}

    /*! Assign operator copies from an exemplar \p std::vector.
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The \p std::vector to copy.
     */
    template<typename OtherT, typename OtherAlloc>
    array1d &operator=(const std::vector<OtherT, OtherAlloc> &v)
    { Parent::operator=(v); return *this; }

    /*! Copy constructor copies from an exemplar std::vector with different type
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The array1d to copy.
     */
    template<typename OtherT, typename OtherMem>
    array1d(const thrust::host_vector<OtherT, OtherMem> &v)
        : Parent(v) {}

    /*! Assign operator copies from an exemplar \p std::vector.
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The \p std::vector to copy.
     */
    template<typename OtherT, typename OtherAlloc>
    array1d &operator=(const thrust::host_vector<OtherT, OtherAlloc> &v)
    { Parent::operator=(v); return *this; }

    /*! Copy constructor copies from an exemplar std::vector with different type
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The array1d to copy.
     */
    template<typename OtherT, typename OtherAlloc>
    array1d(const thrust::device_vector<OtherT, OtherAlloc> &v)
        : Parent(v) {}

    /*! Assign operator copies from an exemplar \p std::vector.
     *  \tparam OtherT Type of std::vector
     *  \tparam OtherAlloc Allocator of std::vector
     *  \param v The \p std::vector to copy.
     */
    template<typename OtherT, typename OtherAlloc>
    array1d &operator=(const thrust::device_vector<OtherT, OtherAlloc> &v)
    { Parent::operator=(v); return *this; }

    /*! This constructor builds a \p array1d vector from a range.
     *  \tparam Iterator iterator type of copy elements
     *  \param first The beginning of the range.
     *  \param last The end of the range.
     */
    template<typename InputIterator>
    array1d(InputIterator first, InputIterator last)
        : Parent(first, last) {}

    /*! This constructor builds a \p array1d vector from a range.
     *  \tparam Iterator iterator type of copy elements
     *  \param first The beginning of the range.
     *  \param last The end of the range.
     */
    template<typename InputIterator>
    array1d(const array1d_view<InputIterator> &v)
        : Parent(v.begin(), v.end()) {}

    /*! Extract a array from a \p array1d container.
     *  \param start_index The starting index of the sub-array.
     *  \param num_entries The number of entries in the sub-array.
     *  \return array1d_view containing elements [start_index,...,start_index+num_entries)
     *
     * \code
     * // include cusp array1d header file
     * #include <cusp/array1d.h>
     * #include <cusp/print.h>
     *
     * int main()
     * {
     *   typedef cusp::array1d<int,cusp::host_memory> Array;
     *   typedef typename Array::view ArrayView;
     *
     *   // Allocate a array of size 2 in "host" memory
     *   Array a(2);
     *
     *   // Set the first element to 0 and second element to 1
     *   a[0] = 0;
     *   a[1] = 1;
     *
     *   // create a view starting from element 1 of length 1
     *   ArrayView first(a.subarray(1,1);
     *   cusp::print(first);
     *
     *   return 0;
     * }
     * \endcode
     */
    view subarray(size_type start_index, size_type num_entries)
    {
        return view(Parent::begin() + start_index, Parent::begin() + start_index + num_entries);
    }

    /*! Extract a const array from a \p array1d container.
     *  \param start_index The starting index of the sub-array.
     *  \param num_entries The number of entries in the sub-array.
     *  \return array1d_view containing elements [start_index,...,start_index+num_entries)
     *
     * \code
     * // include cusp array1d header file
     * #include <cusp/array1d.h>
     * #include <cusp/print.h>
     *
     * int main()
     * {
     *   typedef cusp::array1d<int,cusp::host_memory> Array;
     *   typedef typename Array::const_view ConstArrayView;
     *
     *   // Allocate a array of size 2 in "host" memory
     *   Array a(2);
     *
     *   // Set the first element to 0 and second element to 1
     *   a[0] = 0;
     *   a[1] = 1;
     *
     *   // create a view starting from element 1 of length 1
     *   ConstArrayView first(a.subarray(1,1);
     *   cusp::print(first);
     *
     *   return 0;
     * }
     * \endcode
     */
    const_view subarray(size_type start_index, size_type num_entries) const
    {
        return const_view(Parent::begin() + start_index, Parent::begin() + start_index + num_entries);
    }

}; // end class array1d
/*! \}
 */

/**
 *  \addtogroup array_views Array Views
 *  \ingroup arrays
 *  \{
 */

/**
 * \brief The array1d_view class is a 1D vector container that wraps data from
 * various iterators in a array1d datatype
 *
 * \tparam Iterator The iterator type used to encapsulate the underlying data.
 *
 * \par Overview
 * array1d_view vector is a container that wraps existing iterators in array1d
 * datatypes to interoperate with cusp algorithms. array1d_view datatypes
 * are interoperable with a wide range of iterators exposed by Thrust and the
 * STL library.
 *
 * \par Example
 * \code
 * // include cusp array1d header file
 * #include <cusp/array1d.h>
 * #include <cusp/print.h>
 *
 * int main()
 * {
 *   // Define the container type
 *   typedef cusp::array1d<int, cusp::device_memory Array;
 *
 *   // Get reference to array view type
 *   typedef Array::view View;
 *
 *   // Allocate array1d container with 10 elements
 *   Array array(10,0);
 *
 *   // Create view to the first 5 elements of the array
 *   View first_half(array.begin(), array.begin() + 5);
 *
 *   // Update entries in first_half
 *   first_half[0] = 0; first_half[1] = 1; first_half[2] = 2;
 *
 *   // print the array with updated values
 *   cusp::print(array);
 * }
 * \endcode
 */
template<typename RandomAccessIterator>
class array1d_view : public thrust::iterator_adaptor<array1d_view<RandomAccessIterator>, RandomAccessIterator>
{
private :
    typedef thrust::iterator_adaptor<array1d_view<RandomAccessIterator>, RandomAccessIterator> Parent;

public :

    /*! \cond */
    typedef RandomAccessIterator                                                iterator;
    typedef cusp::array1d_format                                                format;

    typedef typename thrust::iterator_value<RandomAccessIterator>::type         value_type;
    typedef typename thrust::iterator_system<RandomAccessIterator>::type        memory_space;
    typedef typename thrust::iterator_pointer<RandomAccessIterator>::type       pointer;
    typedef typename thrust::iterator_reference<RandomAccessIterator>::type     reference;
    typedef typename thrust::iterator_difference<RandomAccessIterator>::type    difference_type;
    typedef typename thrust::iterator_difference<RandomAccessIterator>::type    size_type;

    typedef typename cusp::array1d<value_type,memory_space>                     container;
    typedef typename cusp::array1d_view<RandomAccessIterator>                   view;
    /*! \endcond */

    friend class thrust::iterator_core_access;

    /*! This constructor creates an empty \p array1d_view vector.
     */
    array1d_view(void)
        : m_size(0), m_capacity(0) {}

    /*! Copy constructor copies from an exemplar array with iterator
     *  \tparam Array Input array type supporting iterators
     *  \param v The vector to copy.
     */
    template<typename ArrayType>
    array1d_view(ArrayType &v)
        : Parent(v.begin()), m_size(v.size()), m_capacity(v.capacity()) {}

    template<typename ArrayType>
    array1d_view(const ArrayType &v)
        : Parent(v.begin()), m_size(v.size()), m_capacity(v.capacity()) {}

    /*! This constructor builds a \p array1d_view vector from a range.
     *  \param begin The beginning of the range.
     *  \param end The end of the range.
     */
    template<typename InputIterator>
    array1d_view(InputIterator begin, InputIterator end)
        : Parent(begin), m_size(end-begin), m_capacity(end-begin) {}

    /*! Assign operator copies from an exemplar \p array1d_view vector.
     *  \param a The \p array1d_view vector to copy.
     *  \return array1d_view copy of input vector
     */
    array1d_view &operator=(const array1d_view& v) {
        this->base_reference()  = v.begin();
        m_size                  = v.size();
        m_capacity              = v.capacity();
        return *this;
    }

    /*! This method returns a reference pointing to the first element of this
     *  array1d_view.
     *  \return The first element of this array1d_view.
     */
    reference front(void) const
    {
        return *begin();
    }

    /*! This method returns a reference referring to the last element of
     *  this array1d_view.
     *  \return The last element of this array1d_view.
     */
    reference back(void) const
    {
        return *(begin() + (size() - 1));
    }

    /*! \brief Subscript access to the data contained in this array1d_view.
     *  \param n The index of the element for which data should be accessed.
     *  \return Read/write reference to data.
     *
     *  This operator allows for easy, array-style, data access.
     *  Note that data access with this operator is unchecked and
     *  out_of_range lookups are not defined.
     */
    reference operator[](size_type n) const
    {
        return *(begin() + n);
    }

    /*! This method returns an iterator pointing to the beginning of
     *  this array1d_view.
     *  \return base iterator
     */
    iterator begin(void) const
    {
        return this->base();
    }

    /*! This method returns an iterator pointing to one element past the
     *  last of this array1d_view.
     *  \return begin() + size().
     */
    iterator end(void) const
    {
        return begin() + m_size;
    }

    /*! Returns the number of elements in this array1d_view.
     */
    size_type size(void) const
    {
        return m_size;
    }

    /*! Returns the number of elements which have been reserved in this
     *  array1d_view.
     */
    size_type capacity(void) const
    {
        return m_capacity;
    }

    // Thrust does not export iterator pointer types so data is not valid
    // see http://thrust.github.io/doc/classthrust_1_1iterator__facade.html
    // pointer data(void)
    // {
    //     return &front();
    // }

    /*! \brief Resizes this array1d_view to the specified number of elements.
     *  \param new_size Number of elements this array1d_view should contain.
     *  \throw std::length_error If n exceeds max_size9).
     *
     *  This method will resize this vector_base to the specified number of
     *  elements. If the number is smaller than this array1d_view's current
     *  size this array1d_view is truncated, otherwise throws an error.
     */
    void resize(size_type new_size)
    {
        if (new_size <= m_capacity)
            m_size = new_size;
        else
            throw cusp::not_implemented_exception("array1d_view cannot resize() larger than capacity()");
    }

    /*! Extract a small vector from a \p array1d_view vector.
     *  \param start_index The starting index of the sub-array.
     *  \param num_entries The number of entries in the sub-array.
     *  \return array1d_view containing elements [start_index,...,start_index+num_entries)
     *
     * \code
     * // include cusp array1d header file
     * #include <cusp/array1d.h>
     * #include <cusp/print.h>
     *
     * int main()
     * {
     *   typedef cusp::array1d<int,cusp::host_memory> Array;
     *   typedef typename Array::view ArrayView;
     *
     *   // Allocate a array of size 2 in "host" memory
     *   Array a(2);
     *
     *   // Set the first element to 0 and second element to 1
     *   a[0] = 0;
     *   a[1] = 1;
     *
     *   ArrayView a_view(a);
     *
     *   // create a view starting from element 1 of length 1
     *   cusp::print(a_view.subarray(1,1));
     * }
     *
     * return 0;
     * \endcode
     */
    view subarray(size_type start_index, size_type num_entries)
    {
        return view(begin() + start_index, begin() + start_index + num_entries);
    }

protected:
    // The size of this array1d_view, in number of elements.
    size_type m_size;

    // The capacity of this array1d_view, in number of elements.
    size_type m_capacity;
};

/**
 * \brief Specialized array1d_view wrapping thrust::counting_iterator
 *
 * \tparam ValueType counting array element type.
 *
 * \par Overview
 * The counting_array view is a simple wrapper around the fancy thrust
 * counting_iterator iterator.
 * \see http://thrust.github.io/doc/classthrust_1_1counting__iterator.html
 *
 * \par Example
 * \code
 * // include cusp array1d header file
 * #include <cusp/array1d.h>
 * #include <cusp/print.h>
 *
 * int main()
 * {
 *   // Define the counting array from 0 to 4
 *   cusp::counting_array<int> v1(5);
 *
 *   // Define the counting array from 6 to 9
 *   cusp::counting_array<int> v2(4,6);
 *
 *   // [0,1,2,3,4]
 *   cusp::print(v1);
 *
 *   // [6,7,8,9]
 *   cusp::print(v2);
 * }
 * \endcode
 */
template <typename ValueType>
class counting_array
    : public cusp::array1d_view< thrust::counting_iterator<ValueType> >
{
private:

    typedef cusp::array1d_view< thrust::counting_iterator<ValueType> > Parent;

public:

    /*! \cond */
    typedef typename Parent::iterator  iterator;
    typedef typename Parent::size_type size_type;
    /*! \endcond */

    /*! This constructor creates a counting_array with a given
     * size starting from a given initial value
     *  \param size The number of entries
     *  \param init The initial starting index to start counting
     */
    counting_array(size_type size, ValueType init = ValueType(0))
        : Parent(iterator(init), iterator(init) + size) {}
};

/**
 * \brief Specialized array1d_view wrapping thrust::constant_iterator
 *
 * \tparam ValueType constant array element type.
 *
 * \par Overview
 * The constant_array view is a simple wrapper around the fancy thrust
 * constant_iterator iterator.
 * \see http://thrust.github.io/doc/classthrust_1_1constant__iterator.html
 *
 * \par Example
 * \code
 * // include cusp array1d header file
 * #include <cusp/array1d.h>
 * #include <cusp/print.h>
 *
 * int main()
 * {
 *   // Define the constant array of length 4 with value 3
 *   cusp::constant_array<int> v1(4,3);
 *
 *   // [3,3,3,3]
 *   cusp::print(v1);
 * }
 * \endcode
 */
template <typename ValueType>
class constant_array
    : public cusp::array1d_view< thrust::constant_iterator<ValueType> >
{
private:

    typedef cusp::array1d_view< thrust::constant_iterator<ValueType> > Parent;

public:

    /*! \cond */
    typedef typename Parent::iterator iterator;
    typedef typename Parent::size_type size_type;
    /*! \endcond */

    /*! This constructor creates a constant_array with a given
     * size starting and value
     *  \param size The number of entries.
     *  \param value The constant value to repeat.
     */
    constant_array(size_type size, ValueType value)
        : Parent(iterator(value), iterator(value) + size) {}
};

/**
 *  This is a convenience function for generating an array1d_view
 *  using iterators
 *  \tparam Iterator iterator type of copy elements
 *  \param first The beginning of the range.
 *  \param last The end of the range.
 *  \return array1d_view constructed using Iterator
 */
template <typename Iterator>
array1d_view<Iterator> make_array1d_view(Iterator first, Iterator last)
{
    return array1d_view<Iterator>(first, last);
}

/**
 *  This is a convenience function for generating an array1d_view
 *  using an array1d
 *  \tparam T value_type of the array
 *  \tparam MemorySpace memory space of the array (cusp::host_memory or cusp::device_memory)
 *  \param v The array1d used to construct array1d_view
 *  \return array1d_view constructed using input array1d
 */
template <typename T, typename MemorySpace>
typename array1d<T,MemorySpace>::view make_array1d_view(array1d<T,MemorySpace>& v)
{
    return make_array1d_view(v.begin(), v.end());
}

/**
 *  This is a convenience function for generating an array1d_view
 *  using an array1d_view
 *  \tparam T value_type of the array
 *  \tparam MemorySpace memory space of the array (cusp::host_memory or cusp::device_memory)
 *  \param v The array1d_view used to construct array1d_view
 *  \return array1d_view constructed using input array1d_view
 */
template <typename Iterator>
typename array1d_view<Iterator>::view make_array1d_view(array1d_view<Iterator>& v)
{
    return make_array1d_view(v.begin(), v.end());
}

/**
 *  This is a convenience function for generating an array1d_view
 *  using an array1d
 *  \tparam T value_type of the array
 *  \tparam MemorySpace memory space of the array (cusp::host_memory or cusp::device_memory)
 *  \param v The array1d used to construct array1d_view
 *  \return constant array1d_view constructed using input array1d
 */
template <typename T, typename MemorySpace>
typename array1d<T,MemorySpace>::const_view make_array1d_view(const array1d<T,MemorySpace>& v)
{
    return make_array1d_view(v.begin(), v.end());
}
/*! \}
 */

} // end namespace cusp

#include <cusp/detail/array1d.inl>

