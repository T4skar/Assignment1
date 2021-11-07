/**
 * pugixml parser - version 1.6
 * --------------------------------------------------------
 * Copyright (C) 2006-2015, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at http://pugixml.org/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

#ifndef SOURCE_PUGIXML_CPP
#define SOURCE_PUGIXML_CPP

#include "pugixml.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifdef PUGIXML_WCHAR_MODE
#    include <wchar.h>
#endif

#ifndef PUGIXML_NO_XPATH
#    include <math.h>
#    include <float.h>
#    ifdef PUGIXML_NO_EXCEPTIONS
#        include <setjmp.h>
#    endif
#endif

#ifndef PUGIXML_NO_STL
#    include <istream>
#    include <ostream>
#    include <string>
#endif

// For placement new
#include <new>

#ifdef _MSC_VER
#    pragma warning(push)
#    pragma warning(disable: 4127) // conditional expression is constant
#    pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#    pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#    pragma warning(disable: 4702) // unreachable code
#    pragma warning(disable: 4996) // this function or variable may be unsafe
#    pragma warning(disable: 4793) // function compiled as native: presence of '_setjmp' makes a function unmanaged
#endif

#ifdef __INTEL_COMPILER
#    pragma warning(disable: 177) // function was declared but never referenced 
#    pragma warning(disable: 279) // controlling expression is constant
#    pragma warning(disable: 1478 1786) // function was declared "deprecated"
#    pragma warning(disable: 1684) // conversion from pointer to same-sized integral type
#endif

#if defined(__BORLANDC__) && defined(PUGIXML_HEADER_ONLY)
#    pragma warn -8080 // symbol is declared but never used; disabling this inside push/pop bracket does not make the warning go away
#endif

#ifdef __BORLANDC__
#    pragma option push
#    pragma warn -8008 // condition is always false
#    pragma warn -8066 // unreachable code
#endif

#ifdef __SNC__
// Using diag_push/diag_pop does not disable the warnings inside templates due to a compiler bug
#    pragma diag_suppress=178 // function was declared but never referenced
#    pragma diag_suppress=237 // controlling expression is constant
#endif

// Inlining controls
#if defined(_MSC_VER) && _MSC_VER >= 1300
#    define PUGI__NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#    define PUGI__NO_INLINE __attribute__((noinline))
#else
#    define PUGI__NO_INLINE 
#endif

// Branch weight controls
#if defined(__GNUC__)
#    define PUGI__UNLIKELY(cond) __builtin_expect(cond, 0)
#else
#    define PUGI__UNLIKELY(cond) (cond)
#endif

// Simple static assertion
#define PUGI__STATIC_ASSERT(cond) { static const char condition_failed[(cond) ? 1 : -1] = {0}; (void)condition_failed[0]; }

// Digital Mars C++ bug workaround for passing char loaded from memory via stack
#ifdef __DMC__
#    define PUGI__DMC_VOLATILE volatile
#else
#    define PUGI__DMC_VOLATILE
#endif

// Borland C++ bug workaround for not defining ::memcpy depending on header include order (can't always use std::memcpy because some compilers don't have it at all)
#if defined(__BORLANDC__) && !defined(__MEM_H_USING_LIST)
using std::memcpy;
using std::memmove;
#endif

// In some environments MSVC is a compiler but the CRT lacks certain MSVC-specific features
#if defined(_MSC_VER) && !defined(__S3E__)
#    define PUGI__MSVC_CRT_VERSION _MSC_VER
#endif

#ifdef PUGIXML_HEADER_ONLY
#    define PUGI__NS_BEGIN namespace pugi { namespace impl {
#    define PUGI__NS_END } }
#    define PUGI__FN inline
#    define PUGI__FN_NO_INLINE inline
#else
#    if defined(_MSC_VER) && _MSC_VER < 1300 // MSVC6 seems to have an amusing bug with anonymous namespaces inside namespaces
#        define PUGI__NS_BEGIN namespace pugi { namespace impl {
#        define PUGI__NS_END } }
#    else
#        define PUGI__NS_BEGIN namespace pugi { namespace impl { namespace {
#        define PUGI__NS_END } } }
#    endif
#    define PUGI__FN
#    define PUGI__FN_NO_INLINE PUGI__NO_INLINE
#endif

// uintptr_t
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#    include <stdint.h>
#else
#    ifndef _UINTPTR_T_DEFINED
// No native uintptr_t in MSVC6 and in some WinCE versions
typedef size_t uintptr_t;
#define _UINTPTR_T_DEFINED
#    endif
PUGI__NS_BEGIN
    typedef unsigned __int8 uint8_t;
    typedef unsigned __int16 uint16_t;
    typedef unsigned __int32 uint32_t;
PUGI__NS_END
#endif

// Memory allocation
PUGI__NS_BEGIN
    PUGI__FN void* default_allocate(size_t size)
    {
        return malloc(size);
    }

    PUGI__FN void default_deallocate(void* ptr)
    {
        free(ptr);
    }

    template <typename T>
    struct xml_memory_management_function_storage
    {
        static allocation_function allocate;
        static deallocation_function deallocate;
    };

    // Global allocation functions are stored in class statics so that in header mode linker deduplicates them
    // Without a template<> we'll get multiple definitions of the same static
    template <typename T> allocation_function xml_memory_management_function_storage<T>::allocate = default_allocate;
    template <typename T> deallocation_function xml_memory_management_function_storage<T>::deallocate = default_deallocate;

    typedef xml_memory_management_function_storage<int> xml_memory;
PUGI__NS_END

// String utilities
PUGI__NS_BEGIN
    // Get string length
    PUGI__FN size_t strlength(const char_t* s)
    {
        assert(s);

    #ifdef PUGIXML_WCHAR_MODE
        return wcslen(s);
    #else
        return strlen(s);
    #endif
    }

    // Compare two strings
    PUGI__FN bool strequal(const char_t* src, const char_t* dst)
    {
        assert(src && dst);

    #ifdef PUGIXML_WCHAR_MODE
        return wcscmp(src, dst) == 0;
    #else
        return strcmp(src, dst) == 0;
    #endif
    }

    // Compare lhs with [rhs_begin, rhs_end)
    PUGI__FN bool strequalrange(const char_t* lhs, const char_t* rhs, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            if (lhs[i] != rhs[i])
                return false;
    
        return lhs[count] == 0;
    }

    // Get length of wide string, even if CRT lacks wide character support
    PUGI__FN size_t strlength_wide(const wchar_t* s)
    {
        assert(s);

    #ifdef PUGIXML_WCHAR_MODE
        return wcslen(s);
    #else
        const wchar_t* end = s;
        while (*end) end++;
        return static_cast<size_t>(end - s);
    #endif
    }

#ifdef PUGIXML_WCHAR_MODE
    // Convert string to wide string, assuming all symbols are ASCII
    PUGI__FN void widen_ascii(wchar_t* dest, const char* source)
    {
        for (const char* i = source; *i; ++i) *dest++ = *i;
        *dest = 0;
    }
#endif
PUGI__NS_END

#if !defined(PUGIXML_NO_STL) || !defined(PUGIXML_NO_XPATH)
// auto_ptr-like buffer holder for exception recovery
PUGI__NS_BEGIN
    struct buffer_holder
    {
        void* data;
        void (*deleter)(void*);

        buffer_holder(void* data_, void (*deleter_)(void*)): data(data_), deleter(deleter_)
        {
        }

        ~buffer_holder()
        {
            if (data) deleter(data);
        }

        void* release()
        {
            void* result = data;
            data = 0;
            return result;
        }
    };
PUGI__NS_END
#endif

PUGI__NS_BEGIN
    static const size_t xml_memory_page_size =
    #ifdef PUGIXML_MEMORY_PAGE_SIZE
        PUGIXML_MEMORY_PAGE_SIZE
    #else
        32768
    #endif
        ;

    static const uintptr_t xml_memory_page_alignment = 64;
    static const uintptr_t xml_memory_page_pointer_mask = ~(xml_memory_page_alignment - 1);
    static const uintptr_t xml_memory_page_contents_shared_mask = 32;
    static const uintptr_t xml_memory_page_name_allocated_mask = 16;
    static const uintptr_t xml_memory_page_value_allocated_mask = 8;
    static const uintptr_t xml_memory_page_type_mask = 7;
    static const uintptr_t xml_memory_page_name_allocated_or_shared_mask = xml_memory_page_name_allocated_mask | xml_memory_page_contents_shared_mask;
    static const uintptr_t xml_memory_page_value_allocated_or_shared_mask = xml_memory_page_value_allocated_mask | xml_memory_page_contents_shared_mask;

    #define PUGI__NODETYPE(n) static_cast<xml_node_type>(((n)->header & impl::xml_memory_page_type_mask) + 1)

    struct xml_allocator;

    struct xml_memory_page
    {
        static xml_memory_page* construct(void* memory)
        {
            xml_memory_page* result = static_cast<xml_memory_page*>(memory);

            result->allocator = 0;
            result->prev = 0;
            result->next = 0;
            result->busy_size = 0;
            result->freed_size = 0;

            return result;
        }

        xml_allocator* allocator;

        xml_memory_page* prev;
        xml_memory_page* next;

        size_t busy_size;
        size_t freed_size;
    };

    struct xml_memory_string_header
    {
        uint16_t page_offset; // offset from page->data
        uint16_t full_size; // 0 if string occupies whole page
    };

    struct xml_allocator
    {
        xml_allocator(xml_memory_page* root): _root(root), _busy_size(root->busy_size)
        {
        }

        xml_memory_page* allocate_page(size_t data_size)
        {
            size_t size = sizeof(xml_memory_page) + data_size;

            // allocate block with some alignment, leaving memory for worst-case padding
            void* memory = xml_memory::allocate(size + xml_memory_page_alignment);
            if (!memory) return 0;

            // align to next page boundary (note: this guarantees at least 1 usable byte before the page)
            char* page_memory = reinterpret_cast<char*>((reinterpret_cast<uintptr_t>(memory) + xml_memory_page_alignment) & ~(xml_memory_page_alignment - 1));

            // prepare page structure
            xml_memory_page* page = xml_memory_page::construct(page_memory);
            assert(page);

            page->allocator = _root->allocator;

            // record the offset for freeing the memory block
            assert(page_memory > memory && page_memory - static_cast<char*>(memory) <= 127);
            page_memory[-1] = static_cast<char>(page_memory - static_cast<char*>(memory));

            return page;
        }

        static void deallocate_page(xml_memory_page* page)
        {
            char* page_memory = reinterpret_cast<char*>(page);

            xml_memory::deallocate(page_memory - page_memory[-1]);
        }

        void* allocate_memory_oob(size_t size, xml_memory_page*& out_page);

        void* allocate_memory(size_t size, xml_memory_page*& out_page)
        {
            if (_busy_size + size > xml_memory_page_size) return allocate_memory_oob(size, out_page);

            void* buf = reinterpret_cast<char*>(_root) + sizeof(xml_memory_page) + _busy_size;

            _busy_size += size;

            out_page = _root;

            return buf;
        }

        void deallocate_memory(void* ptr, size_t size, xml_memory_page* page)
        {
            if (page == _root) page->busy_size = _busy_size;

            assert(ptr >= reinterpret_cast<char*>(page) + sizeof(xml_memory_page) && ptr < reinterpret_cast<char*>(page) + sizeof(xml_memory_page) + page->busy_size);
            (void)!ptr;

            page->freed_size += size;
            assert(page->freed_size <= page->busy_size);

            if (page->freed_size == page->busy_size)
            {
                if (page->next == 0)
                {
                    assert(_root == page);

                    // top page freed, just reset sizes
                    page->busy_size = page->freed_size = 0;
                    _busy_size = 0;
                }
                else
                {
                    assert(_root != page);
                    assert(page->prev);

                    // remove from the list
                    page->prev->next = page->next;
                    page->next->prev = page->prev;

                    // deallocate
                    deallocate_page(page);
                }
            }
        }

        char_t* allocate_string(size_t length)
        {
            static const size_t max_encoded_offset = (1 << 16) * sizeof(void*);

            PUGI__STATIC_ASSERT(xml_memory_page_size <= max_encoded_offset);

            // allocate memory for string and header block
            size_t size = sizeof(xml_memory_string_header) + length * sizeof(char_t);
            
            // round size up to pointer alignment boundary
            size_t full_size = (size + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

            xml_memory_page* page;
            xml_memory_string_header* header = static_cast<xml_memory_string_header*>(allocate_memory(full_size, page));

            if (!header) return 0;

            // setup header
            ptrdiff_t page_offset = reinterpret_cast<char*>(header) - reinterpret_cast<char*>(page) - sizeof(xml_memory_page);

            assert(page_offset % sizeof(void*) == 0);
            assert(page_offset >= 0 && static_cast<size_t>(page_offset) < max_encoded_offset);
            header->page_offset = static_cast<uint16_t>(static_cast<size_t>(page_offset) / sizeof(void*));

            // full_size == 0 for large strings that occupy the whole page
            assert(full_size % sizeof(void*) == 0);
            assert(full_size < max_encoded_offset || (page->busy_size == full_size && page_offset == 0));
            header->full_size = static_cast<uint16_t>(full_size < max_encoded_offset ? full_size / sizeof(void*) : 0);

            // round-trip through void* to avoid 'cast increases required alignment of target type' warning
            // header is guaranteed a pointer-sized alignment, which should be enough for char_t
            return static_cast<char_t*>(static_cast<void*>(header + 1));
        }

        void deallocate_string(char_t* string)
        {
            // this function casts pointers through void* to avoid 'cast increases required alignment of target type' warnings
            // we're guaranteed the proper (pointer-sized) alignment on the input string if it was allocated via allocate_string

            // get header
            xml_memory_string_header* header = static_cast<xml_memory_string_header*>(static_cast<void*>(string)) - 1;
            assert(header);

            // deallocate
            size_t page_offset = sizeof(xml_memory_page) + header->page_offset * sizeof(void*);
            xml_memory_page* page = reinterpret_cast<xml_memory_page*>(static_cast<void*>(reinterpret_cast<char*>(header) - page_offset));

            // if full_size == 0 then this string occupies the whole page
            size_t full_size = header->full_size == 0 ? page->busy_size : header->full_size * sizeof(void*);

            deallocate_memory(header, full_size, page);
        }

        xml_memory_page* _root;
        size_t _busy_size;
    };

    PUGI__FN_NO_INLINE void* xml_allocator::allocate_memory_oob(size_t size, xml_memory_page*& out_page)
    {
        const size_t large_allocation_threshold = xml_memory_page_size / 4;

        xml_memory_page* page = allocate_page(size <= large_allocation_threshold ? xml_memory_page_size : size);
        out_page = page;

        if (!page) return 0;

        if (size <= large_allocation_threshold)
        {
            _root->busy_size = _busy_size;

            // insert page at the end of linked list
            page->prev = _root;
            _root->next = page;
            _root = page;

            _busy_size = size;
        }
        else
        {
            // insert page before the end of linked list, so that it is deleted as soon as possible
            // the last page is not deleted even if it's empty (see deallocate_memory)
            assert(_root->prev);

            page->prev = _root->prev;
            page->next = _root;

            _root->prev->next = page;
            _root->prev = page;
        }

        // allocate inside page
        page->busy_size = size;

        return reinterpret_cast<char*>(page) + sizeof(xml_memory_page);
    }
PUGI__NS_END

namespace pugi
{
    /// A 'name=value' XML attribute structure.
    struct xml_attribute_struct
    {
        /// Default ctor
        xml_attribute_struct(impl::xml_memory_page* page): header(reinterpret_cast<uintptr_t>(page)), name(0), value(0), prev_attribute_c(0), next_attribute(0)
        {
        }

        uintptr_t header;

        char_t* name;    ///< Pointer to attribute name.
        char_t*    value;    ///< Pointer to attribute value.

        xml_attribute_struct* prev_attribute_c;    ///< Previous attribute (cyclic list)
        xml_attribute_struct* next_attribute;    ///< Next attribute
    };

    /// An XML document tree node.
    struct xml_node_struct
    {
        /// Default ctor
        /// \param type - node type
        xml_node_struct(impl::xml_memory_page* page, xml_node_type type): header(reinterpret_cast<uintptr_t>(page) | (type - 1)), parent(0), name(0), value(0), first_child(0), prev_sibling_c(0), next_sibling(0), first_attribute(0)
        {
        }

        uintptr_t header;

        xml_node_struct*        parent;                    ///< Pointer to parent

        char_t*                    name;                    ///< Pointer to element name.
        char_t*                    value;                    ///< Pointer to any associated string data.

        xml_node_struct*        first_child;            ///< First child
        
        xml_node_struct*        prev_sibling_c;            ///< Left brother (cyclic list)
        xml_node_struct*        next_sibling;            ///< Right brother
        
        xml_attribute_struct*    first_attribute;        ///< First attribute
    };
}

PUGI__NS_BEGIN
    struct xml_extra_buffer
    {
        char_t* buffer;
        xml_extra_buffer* next;
    };

    struct xml_document_struct: public xml_node_struct, public xml_allocator
    {
        xml_document_struct(xml_memory_page* page): xml_node_struct(page, node_document), xml_allocator(page), buffer(0), extra_buffers(0)
        {
        }

        const char_t* buffer;

        xml_extra_buffer* extra_buffers;
    };

    inline xml_allocator& get_allocator(const xml_node_struct* node)
    {
        assert(node);

        return *reinterpret_cast<xml_memory_page*>(node->header & xml_memory_page_pointer_mask)->allocator;
    }

    template <typename Object> inline xml_document_struct& get_document(const Object* object)
    {
        assert(object);

        return *static_cast<xml_document_struct*>(reinterpret_cast<xml_memory_page*>(object->header & xml_memory_page_pointer_mask)->allocator);
    }
PUGI__NS_END

// Low-level DOM operations
PUGI__NS_BEGIN
    inline xml_attribute_struct* allocate_attribute(xml_allocator& alloc)
    {
        xml_memory_page* page;
        void* memory = alloc.allocate_memory(sizeof(xml_attribute_struct), page);

        return new (memory) xml_attribute_struct(page);
    }

    inline xml_node_struct* allocate_node(xml_allocator& alloc, xml_node_type type)
    {
        xml_memory_page* page;
        void* memory = alloc.allocate_memory(sizeof(xml_node_struct), page);

        return new (memory) xml_node_struct(page, type);
    }

    inline void destroy_attribute(xml_attribute_struct* a, xml_allocator& alloc)
    {
        uintptr_t header = a->header;

        if (header & impl::xml_memory_page_name_allocated_mask) alloc.deallocate_string(a->name);
        if (header & impl::xml_memory_page_value_allocated_mask) alloc.deallocate_string(a->value);

        alloc.deallocate_memory(a, sizeof(xml_attribute_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
    }

    inline void destroy_node(xml_node_struct* n, xml_allocator& alloc)
    {
        uintptr_t header = n->header;

        if (header & impl::xml_memory_page_name_allocated_mask) alloc.deallocate_string(n->name);
        if (header & impl::xml_memory_page_value_allocated_mask) alloc.deallocate_string(n->value);

        for (xml_attribute_struct* attr = n->first_attribute; attr; )
        {
            xml_attribute_struct* next = attr->next_attribute;

            destroy_attribute(attr, alloc);

            attr = next;
        }

        for (xml_node_struct* child = n->first_child; child; )
        {
            xml_node_struct* next = child->next_sibling;

            destroy_node(child, alloc);

            child = next;
        }

        alloc.deallocate_memory(n, sizeof(xml_node_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
    }

    inline void append_node(xml_node_struct* child, xml_node_struct* node)
    {
        child->parent = node;

        xml_node_struct* head = node->first_child;

        if (head)
        {
            xml_node_struct* tail = head->prev_sibling_c;

            tail->next_sibling = child;
            child->prev_sibling_c = tail;
            head->prev_sibling_c = child;
        }
        else
        {
            node->first_child = child;
            child->prev_sibling_c = child;
        }
    }

    inline void prepend_node(xml_node_struct* child, xml_node_struct* node)
    {
        child->parent = node;

        xml_node_struct* head = node->first_child;

        if (head)
        {
            child->prev_sibling_c = head->prev_sibling_c;
            head->prev_sibling_c = child;
        }
        else
            child->prev_sibling_c = child;

        child->next_sibling = head;
        node->first_child = child;
    }

    inline void insert_node_after(xml_node_struct* child, xml_node_struct* node)
    {
        xml_node_struct* parent = node->parent;

        child->parent = parent;

        if (node->next_sibling)
            node->next_sibling->prev_sibling_c = child;
        else
            parent->first_child->prev_sibling_c = child;

        child->next_sibling = node->next_sibling;
        child->prev_sibling_c = node;

        node->next_sibling = child;
    }

    inline void insert_node_before(xml_node_struct* child, xml_node_struct* node)
    {
        xml_node_struct* parent = node->parent;

        child->parent = parent;

        if (node->prev_sibling_c->next_sibling)
            node->prev_sibling_c->next_sibling = child;
        else
            parent->first_child = child;

        child->prev_sibling_c = node->prev_sibling_c;
        child->next_sibling = node;

        node->prev_sibling_c = child;
    }

    inline void remove_node(xml_node_struct* node)
    {
        xml_node_struct* parent = node->parent;

        if (node->next_sibling)
            node->next_sibling->prev_sibling_c = node->prev_sibling_c;
        else
            parent->first_child->prev_sibling_c = node->prev_sibling_c;

        if (node->prev_sibling_c->next_sibling)
            node->prev_sibling_c->next_sibling = node->next_sibling;
        else
            parent->first_child = node->next_sibling;

        node->parent = 0;
        node->prev_sibling_c = 0;
        node->next_sibling = 0;
    }

    inline void append_attribute(xml_attribute_struct* attr, xml_node_struct* node)
    {
        xml_attribute_struct* head = node->first_attribute;

        if (head)
        {
            xml_attribute_struct* tail = head->prev_attribute_c;

            tail->next_attribute = attr;
            attr->prev_attribute_c = tail;
            head->prev_attribute_c = attr;
        }
        else
        {
            node->first_attribute = attr;
            attr->prev_attribute_c = attr;
        }
    }

    inline void prepend_attribute(xml_attribute_struct* attr, xml_node_struct* node)
    {
        xml_attribute_struct* head = node->first_attribute;

        if (head)
        {
            attr->prev_attribute_c = head->prev_attribute_c;
            head->prev_attribute_c = attr;
        }
        else
            attr->prev_attribute_c = attr;

        attr->next_attribute = head;
        node->first_attribute = attr;
    }

    inline void insert_attribute_after(xml_attribute_struct* attr, xml_attribute_struct* place, xml_node_struct* node)
    {
        if (place->next_attribute)
            place->next_attribute->prev_attribute_c = attr;
        else
            node->first_attribute->prev_attribute_c = attr;

        attr->next_attribute = place->next_attribute;
        attr->prev_attribute_c = place;
        place->next_attribute = attr;
    }

    inline void insert_attribute_before(xml_attribute_struct* attr, xml_attribute_struct* place, xml_node_struct* node)
    {
        if (place->prev_attribute_c->next_attribute)
            place->prev_attribute_c->next_attribute = attr;
        else
            node->first_attribute = attr;

        attr->prev_attribute_c = place->prev_attribute_c;
        attr->next_attribute = place;
        place->prev_attribute_c = attr;
    }

    inline void remove_attribute(xml_attribute_struct* attr, xml_node_struct* node)
    {
        if (attr->next_attribute)
            attr->next_attribute->prev_attribute_c = attr->prev_attribute_c;
        else
            node->first_attribute->prev_attribute_c = attr->prev_attribute_c;

        if (attr->prev_attribute_c->next_attribute)
            attr->prev_attribute_c->next_attribute = attr->next_attribute;
        else
            node->first_attribute = attr->next_attribute;

        attr->prev_attribute_c = 0;
        attr->next_attribute = 0;
    }

    PUGI__FN_NO_INLINE xml_node_struct* append_new_node(xml_node_struct* node, xml_allocator& alloc, xml_node_type type = node_element)
    {
        xml_node_struct* child = allocate_node(alloc, type);
        if (!child) return 0;

        append_node(child, node);

        return child;
    }

    PUGI__FN_NO_INLINE xml_attribute_struct* append_new_attribute(xml_node_struct* node, xml_allocator& alloc)
    {
        xml_attribute_struct* attr = allocate_attribute(alloc);
        if (!attr) return 0;

        append_attribute(attr, node);

        return attr;
    }
PUGI__NS_END

// Helper classes for code generation
PUGI__NS_BEGIN
    struct opt_false
    {
        enum { value = 0 };
    };

    struct opt_true
    {
        enum { value = 1 };
    };
PUGI__NS_END

// Unicode utilities
PUGI__NS_BEGIN
    inline uint16_t endian_swap(uint16_t value)
    {
        return static_cast<uint16_t>(((value & 0xff) << 8) | (value >> 8));
    }

    inline uint32_t endian_swap(uint32_t value)
    {
        return ((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
    }

    struct utf8_counter
    {
        typedef size_t value_type;

        static value_type low(value_type result, uint32_t ch)
        {
            // U+0000..U+007F
            if (ch < 0x80) return result + 1;
            // U+0080..U+07FF
            else if (ch < 0x800) return result + 2;
            // U+0800..U+FFFF
            else return result + 3;
        }

        static value_type high(value_type result, uint32_t)
        {
            // U+10000..U+10FFFF
            return result + 4;
        }
    };

    struct utf8_writer
    {
        typedef uint8_t* value_type;

        static value_type low(value_type result, uint32_t ch)
        {
            // U+0000..U+007F
            if (ch < 0x80)
            {
                *result = static_cast<uint8_t>(ch);
                return result + 1;
            }
            // U+0080..U+07FF
            else if (ch < 0x800)
            {
                result[0] = static_cast<uint8_t>(0xC0 | (ch >> 6));
                result[1] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
                return result + 2;
            }
            // U+0800..U+FFFF
            else
            {
                result[0] = static_cast<uint8_t>(0xE0 | (ch >> 12));
                result[1] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
                result[2] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
                return result + 3;
            }
        }

        static value_type high(value_type result, uint32_t ch)
        {
            // U+10000..U+10FFFF
            result[0] = static_cast<uint8_t>(0xF0 | (ch >> 18));
            result[1] = static_cast<uint8_t>(0x80 | ((ch >> 12) & 0x3F));
            result[2] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
            result[3] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
            return result + 4;
        }

        static value_type any(value_type result, uint32_t ch)
        {
            return (ch < 0x10000) ? low(result, ch) : high(result, ch);
        }
    };

    struct utf16_counter
    {
        typedef size_t value_type;

        static value_type low(value_type result, uint32_t)
        {
            return result + 1;
        }

        static value_type high(value_type result, uint32_t)
        {
            return result + 2;
        }
    };

    struct utf16_writer
    {
        typedef uint16_t* value_type;

        static value_type low(value_type result, uint32_t ch)
        {
            *result = static_cast<uint16_t>(ch);

            return result + 1;
        }

        static value_type high(value_type result, uint32_t ch)
        {
            uint32_t msh = static_cast<uint32_t>(ch - 0x10000) >> 10;
            uint32_t lsh = static_cast<uint32_t>(ch - 0x10000) & 0x3ff;

            result[0] = static_cast<uint16_t>(0xD800 + msh);
            result[1] = static_cast<uint16_t>(0xDC00 + lsh);

            return result + 2;
        }

        static value_type any(value_type result, uint32_t ch)
        {
            return (ch < 0x10000) ? low(result, ch) : high(result, ch);
        }
    };

    struct utf32_counter
    {
        typedef size_t value_type;

        static value_type low(value_type result, uint32_t)
        {
            return result + 1;
        }

        static value_type high(value_type result, uint32_t)
        {
            return result + 1;
        }
    };

    struct utf32_writer
    {
        typedef uint32_t* value_type;

        static value_type low(value_type result, uint32_t ch)
        {
            *result = ch;

            return result + 1;
        }

        static value_type high(value_type result, uint32_t ch)
        {
            *result = ch;

            return result + 1;
        }

        static value_type any(value_type result, uint32_t ch)
        {
            *result = ch;

            return result + 1;
        }
    };

    struct latin1_writer
    {
        typedef uint8_t* value_type;

        static value_type low(value_type result, uint32_t ch)
        {
            *result = static_cast<uint8_t>(ch > 255 ? '?' : ch);

            return result + 1;
        }

        static value_type high(value_type result, uint32_t ch)
        {
            (void)ch;

            *result = '?';

            return result + 1;
        }
    };

    template <size_t size> struct wchar_selector;

    template <> struct wchar_selector<2>
    {
        typedef uint16_t type;
        typedef utf16_counter counter;
        typedef utf16_writer writer;
    };

    template <> struct wchar_selector<4>
    {
        typedef uint32_t type;
        typedef utf32_counter counter;
        typedef utf32_writer writer;
    };

    typedef wchar_selector<sizeof(wchar_t)>::counter wchar_counter;
    typedef wchar_selector<sizeof(wchar_t)>::writer wchar_writer;

    template <typename Traits, typename opt_swap = opt_false> struct utf_decoder
    {
        static inline typename Traits::value_type decode_utf8_block(const uint8_t* data, size_t size, typename Traits::value_type result)
        {
            const uint8_t utf8_byte_mask = 0x3f;

            while (size)
            {
                uint8_t lead = *data;

                // 0xxxxxxx -> U+0000..U+007F
                if (lead < 0x80)
                {
                    result = Traits::low(result, lead);
                    data += 1;
                    size -= 1;

                    // process aligned single-byte (ascii) blocks
                    if ((reinterpret_cast<uintptr_t>(data) & 3) == 0)
                    {
                        // round-trip through void* to silence 'cast increases required alignment of target type' warnings
                        while (size >= 4 && (*static_cast<const uint32_t*>(static_cast<const void*>(data)) & 0x80808080) == 0)
                        {
                            result = Traits::low(result, data[0]);
                            result = Traits::low(result, data[1]);
                            result = Traits::low(result, data[2]);
                            result = Traits::low(result, data[3]);
                            data += 4;
                            size -= 4;
                        }
                    }
                }
                // 110xxxxx -> U+0080..U+07FF
                else if (static_cast<unsigned int>(lead - 0xC0) < 0x20 && size >= 2 && (data[1] & 0xc0) == 0x80)
                {
                    result = Traits::low(result, ((lead & ~0xC0) << 6) | (data[1] & utf8_byte_mask));
                    data += 2;
                    size -= 2;
                }
                // 1110xxxx -> U+0800-U+FFFF
                else if (static_cast<unsigned int>(lead - 0xE0) < 0x10 && size >= 3 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80)
                {
                    result = Traits::low(result, ((lead & ~0xE0) << 12) | ((data[1] & utf8_byte_mask) << 6) | (data[2] & utf8_byte_mask));
                    data += 3;
                    size -= 3;
                }
                // 11110xxx -> U+10000..U+10FFFF
                else if (static_cast<unsigned int>(lead - 0xF0) < 0x08 && size >= 4 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80 && (data[3] & 0xc0) == 0x80)
                {
                    result = Traits::high(result, ((lead & ~0xF0) << 18) | ((data[1] & utf8_byte_mask) << 12) | ((data[2] & utf8_byte_mask) << 6) | (data[3] & utf8_byte_mask));
                    data += 4;
                    size -= 4;
                }
                // 10xxxxxx or 11111xxx -> invalid
                else
                {
                    data += 1;
                    size -= 1;
                }
            }

            return result;
        }

        static inline typename Traits::value_type decode_utf16_block(const uint16_t* data, size_t size, typename Traits::value_type result)
        {
            const uint16_t* end = data + size;

            while (data < end)
            {
                unsigned int lead = opt_swap::value ? endian_swap(*data) : *data;

                // U+0000..U+D7FF
                if (lead < 0xD800)
                {
                    result = Traits::low(result, lead);
                    data += 1;
                }
                // U+E000..U+FFFF
                else if (static_cast<unsigned int>(lead - 0xE000) < 0x2000)
                {
                    result = Traits::low(result, lead);
                    data += 1;
                }
                // surrogate pair lead
                else if (static_cast<unsigned int>(lead - 0xD800) < 0x400 && data + 1 < end)
                {
                    uint16_t next = opt_swap::value ? endian_swap(data[1]) : data[1];

                    if (static_cast<unsigned int>(next - 0xDC00) < 0x400)
                    {
                        result = Traits::high(result, 0x10000 + ((lead & 0x3ff) << 10) + (next & 0x3ff));
                        data += 2;
                    }
                    else
                    {
                        data += 1;
                    }
                }
                else
                {
                    data += 1;
                }
            }

            return result;
        }

        static inline typename Traits::value_type decode_utf32_block(const uint32_t* data, size_t size, typename Traits::value_type result)
        {
            const uint32_t* end = data + size;

            while (data < end)
            {
                uint32_t lead = opt_swap::value ? endian_swap(*data) : *data;

                // U+0000..U+FFFF
                if (lead < 0x10000)
                {
                    result = Traits::low(result, lead);
                    data += 1;
                }
                // U+10000..U+10FFFF
                else
                {
                    result = Traits::high(result, lead);
                    data += 1;
                }
            }

            return result;
        }

        static inline typename Traits::value_type decode_latin1_block(const uint8_t* data, size_t size, typename Traits::value_type result)
        {
            for (size_t i = 0; i < size; ++i)
            {
                result = Traits::low(result, data[i]);
            }

            return result;
        }

        static inline typename Traits::value_type decode_wchar_block_impl(const uint16_t* data, size_t size, typename Traits::value_type result)
        {
            return decode_utf16_block(data, size, result);
        }

        static inline typename Traits::value_type decode_wchar_block_impl(const uint32_t* data, size_t size, typename Traits::value_type result)
        {
            return decode_utf32_block(data, size, result);
        }

        static inline typename Traits::value_type decode_wchar_block(const wchar_t* data, size_t size, typename Traits::value_type result)
        {
            return decode_wchar_block_impl(reinterpret_cast<const wchar_selector<sizeof(wchar_t)>::type*>(data), size, result);
        }
    };

    template <typename T> PUGI__FN void convert_utf_endian_swap(T* result, const T* data, size_t length)
    {
        for (size_t i = 0; i < length; ++i) result[i] = endian_swap(data[i]);
    }

#ifdef PUGIXML_WCHAR_MODE
    PUGI__FN void convert_wchar_endian_swap(wchar_t* result, const wchar_t* data, size_t length)
    {
        for (size_t i = 0; i < length; ++i) result[i] = static_cast<wchar_t>(endian_swap(static_cast<wchar_selector<sizeof(wchar_t)>::type>(data[i])));
    }
#endif
PUGI__NS_END

PUGI__NS_BEGIN
    enum chartype_t
    {
        ct_parse_pcdata = 1,    // \0, &, \r, <
        ct_parse_attr = 2,        // \0, &, \r, ', "
        ct_parse_attr_ws = 4,    // \0, &, \r, ', ", \n, tab
        ct_space = 8,            // \r, \n, space, tab
        ct_parse_cdata = 16,    // \0, ], >, \r
        ct_parse_comment = 32,    // \0, -, >, \r
        ct_symbol = 64,            // Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
        ct_start_symbol = 128    // Any symbol > 127, a-z, A-Z, _, :
    };

    static const unsigned char chartype_table[256] =
    {
        55,  0,   0,   0,   0,   0,   0,   0,      0,   12,  12,  0,   0,   63,  0,   0,   // 0-15
        0,   0,   0,   0,   0,   0,   0,   0,      0,   0,   0,   0,   0,   0,   0,   0,   // 16-31
        8,   0,   6,   0,   0,   0,   7,   6,      0,   0,   0,   0,   0,   96,  64,  0,   // 32-47
        64,  64,  64,  64,  64,  64,  64,  64,     64,  64,  192, 0,   1,   0,   48,  0,   // 48-63
        0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 64-79
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0,   0,   16,  0,   192, // 80-95
        0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 96-111
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0, 0, 0, 0, 0,           // 112-127

        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 128+
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
        192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192
    };

    enum chartypex_t
    {
        ctx_special_pcdata = 1,   // Any symbol >= 0 and < 32 (except \t, \r, \n), &, <, >
        ctx_special_attr = 2,     // Any symbol >= 0 and < 32 (except \t), &, <, >, "
        ctx_start_symbol = 4,      // Any symbol > 127, a-z, A-Z, _
        ctx_digit = 8,              // 0-9
        ctx_symbol = 16              // Any symbol > 127, a-z, A-Z, 0-9, _, -, .
    };
    
    static const unsigned char chartypex_table[256] =
    {
        3,  3,  3,  3,  3,  3,  3,  3,     3,  0,  2,  3,  3,  2,  3,  3,     // 0-15
        3,  3,  3,  3,  3,  3,  3,  3,     3,  3,  3,  3,  3,  3,  3,  3,     // 16-31
        0,  0,  2,  0,  0,  0,  3,  0,     0,  0,  0,  0,  0, 16, 16,  0,     // 32-47
        24, 24, 24, 24, 24, 24, 24, 24,    24, 24, 0,  0,  3,  0,  3,  0,     // 48-63

        0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 64-79
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  20,    // 80-95
        0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 96-111
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  0,     // 112-127

        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 128+
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
        20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20
    };
    
#ifdef PUGIXML_WCHAR_MODE
    #define PUGI__IS_CHARTYPE_IMPL(c, ct, table) ((static_cast<unsigned int>(c) < 128 ? table[static_cast<unsigned int>(c)] : table[128]) & (ct))
#else
    #define PUGI__IS_CHARTYPE_IMPL(c, ct, table) (table[static_cast<unsigned char>(c)] & (ct))
#endif

    #define PUGI__IS_CHARTYPE(c, ct) PUGI__IS_CHARTYPE_IMPL(c, ct, chartype_table)
    #define PUGI__IS_CHARTYPEX(c, ct) PUGI__IS_CHARTYPE_IMPL(c, ct, chartypex_table)

    PUGI__FN bool is_little_endian()
    {
        unsigned int ui = 1;

        return *reinterpret_cast<unsigned char*>(&ui) == 1;
    }

    PUGI__FN xml_encoding get_wchar_encoding()
    {
        PUGI__STATIC_ASSERT(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

        if (sizeof(wchar_t) == 2)
            return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;
        else 
            return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;
    }

    PUGI__FN xml_encoding guess_buffer_encoding(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
    {
        // look for BOM in first few bytes
        if (d0 == 0 && d1 == 0 && d2 == 0xfe && d3 == 0xff) return encoding_utf32_be;
        if (d0 == 0xff && d1 == 0xfe && d2 == 0 && d3 == 0) return encoding_utf32_le;
        if (d0 == 0xfe && d1 == 0xff) return encoding_utf16_be;
        if (d0 == 0xff && d1 == 0xfe) return encoding_utf16_le;
        if (d0 == 0xef && d1 == 0xbb && d2 == 0xbf) return encoding_utf8;

        // look for <, <? or <?xm in various encodings
        if (d0 == 0 && d1 == 0 && d2 == 0 && d3 == 0x3c) return encoding_utf32_be;
        if (d0 == 0x3c && d1 == 0 && d2 == 0 && d3 == 0) return encoding_utf32_le;
        if (d0 == 0 && d1 == 0x3c && d2 == 0 && d3 == 0x3f) return encoding_utf16_be;
        if (d0 == 0x3c && d1 == 0 && d2 == 0x3f && d3 == 0) return encoding_utf16_le;
        if (d0 == 0x3c && d1 == 0x3f && d2 == 0x78 && d3 == 0x6d) return encoding_utf8;

        // look for utf16 < followed by node name (this may fail, but is better than utf8 since it's zero terminated so early)
        if (d0 == 0 && d1 == 0x3c) return encoding_utf16_be;
        if (d0 == 0x3c && d1 == 0) return encoding_utf16_le;

        // no known BOM detected, assume utf8
        return encoding_utf8;
    }

    PUGI__FN xml_encoding get_buffer_encoding(xml_encoding encoding, const void* contents, size_t size)
    {
        // replace wchar encoding with utf implementation
        if (encoding == encoding_wchar) return get_wchar_encoding();

        // replace utf16 encoding with utf16 with specific endianness
        if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

        // replace utf32 encoding with utf32 with specific endianness
        if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

        // only do autodetection if no explicit encoding is requested
        if (encoding != encoding_auto) return encoding;

        // skip encoding autodetection if input buffer is too small
        if (size < 4) return encoding_utf8;

        // try to guess encoding (based on XML specification, Appendix F.1)
        const uint8_t* data = static_cast<const uint8_t*>(contents);

        PUGI__DMC_VOLATILE uint8_t d0 = data[0], d1 = data[1], d2 = data[2], d3 = data[3];

        return guess_buffer_encoding(d0, d1, d2, d3);
    }

    PUGI__FN bool get_mutable_buffer(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
    {
        size_t length = size / sizeof(char_t);

        if (is_mutable)
        {
            out_buffer = static_cast<char_t*>(const_cast<void*>(contents));
            out_length = length;
        }
        else
        {
            char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
            if (!buffer) return false;

            if (contents)
                memcpy(buffer, contents, length * sizeof(char_t));
            else
                assert(length == 0);

            buffer[length] = 0;

            out_buffer = buffer;
            out_length = length + 1;
        }

        return true;
    }

#ifdef PUGIXML_WCHAR_MODE
    PUGI__FN bool need_endian_swap_utf(xml_encoding le, xml_encoding re)
    {
        return (le == encoding_utf16_be && re == encoding_utf16_le) || (le == encoding_utf16_le && re == encoding_utf16_be) ||
               (le == encoding_utf32_be && re == encoding_utf32_le) || (le == encoding_utf32_le && re == encoding_utf32_be);
    }

    PUGI__FN bool convert_buffer_endian_swap(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
    {
        const char_t* data = static_cast<const char_t*>(contents);
        size_t length = size / sizeof(char_t);

        if (is_mutable)
        {
            char_t* buffer = const_cast<char_t*>(data);

            convert_wchar_endian_swap(buffer, data, length);

            out_buffer = buffer;
            out_length = length;
        }
        else
        {
            char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
            if (!buffer) return false;

            convert_wchar_endian_swap(buffer, data, length);
            buffer[length] = 0;

            out_buffer = buffer;
            out_length = length + 1;
        }

        return true;
    }

    PUGI__FN bool convert_buffer_utf8(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size)
    {
        const uint8_t* data = static_cast<const uint8_t*>(contents);
        size_t data_length = size;

        // first pass: get length in wchar_t units
        size_t length = utf_decoder<wchar_counter>::decode_utf8_block(data, data_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert utf8 input to wchar_t
        wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
        wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_utf8_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    template <typename opt_swap> PUGI__FN bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
    {
        const uint16_t* data = static_cast<const uint16_t*>(contents);
        size_t data_length = size / sizeof(uint16_t);

        // first pass: get length in wchar_t units
        size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert utf16 input to wchar_t
        wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
        wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    template <typename opt_swap> PUGI__FN bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
    {
        const uint32_t* data = static_cast<const uint32_t*>(contents);
        size_t data_length = size / sizeof(uint32_t);

        // first pass: get length in wchar_t units
        size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert utf32 input to wchar_t
        wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
        wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    PUGI__FN bool convert_buffer_latin1(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size)
    {
        const uint8_t* data = static_cast<const uint8_t*>(contents);
        size_t data_length = size;

        // get length in wchar_t units
        size_t length = data_length;

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // convert latin1 input to wchar_t
        wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
        wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_latin1_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    PUGI__FN bool convert_buffer(char_t*& out_buffer, size_t& out_length, xml_encoding encoding, const void* contents, size_t size, bool is_mutable)
    {
        // get native encoding
        xml_encoding wchar_encoding = get_wchar_encoding();

        // fast path: no conversion required
        if (encoding == wchar_encoding) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

        // only endian-swapping is required
        if (need_endian_swap_utf(encoding, wchar_encoding)) return convert_buffer_endian_swap(out_buffer, out_length, contents, size, is_mutable);

        // source encoding is utf8
        if (encoding == encoding_utf8) return convert_buffer_utf8(out_buffer, out_length, contents, size);

        // source encoding is utf16
        if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
        {
            xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

            return (native_encoding == encoding) ?
                convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
                convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
        }

        // source encoding is utf32
        if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
        {
            xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

            return (native_encoding == encoding) ?
                convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
                convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
        }

        // source encoding is latin1
        if (encoding == encoding_latin1) return convert_buffer_latin1(out_buffer, out_length, contents, size);

        assert(!"Invalid encoding");
        return false;
    }
#else
    template <typename opt_swap> PUGI__FN bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
    {
        const uint16_t* data = static_cast<const uint16_t*>(contents);
        size_t data_length = size / sizeof(uint16_t);

        // first pass: get length in utf8 units
        size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert utf16 input to utf8
        uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
        uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    template <typename opt_swap> PUGI__FN bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
    {
        const uint32_t* data = static_cast<const uint32_t*>(contents);
        size_t data_length = size / sizeof(uint32_t);

        // first pass: get length in utf8 units
        size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert utf32 input to utf8
        uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
        uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    PUGI__FN size_t get_latin1_7bit_prefix_length(const uint8_t* data, size_t size)
    {
        for (size_t i = 0; i < size; ++i)
            if (data[i] > 127)
                return i;

        return size;
    }

    PUGI__FN bool convert_buffer_latin1(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
    {
        const uint8_t* data = static_cast<const uint8_t*>(contents);
        size_t data_length = size;

        // get size of prefix that does not need utf8 conversion
        size_t prefix_length = get_latin1_7bit_prefix_length(data, data_length);
        assert(prefix_length <= data_length);

        const uint8_t* postfix = data + prefix_length;
        size_t postfix_length = data_length - prefix_length;

        // if no conversion is needed, just return the original buffer
        if (postfix_length == 0) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

        // first pass: get length in utf8 units
        size_t length = prefix_length + utf_decoder<utf8_counter>::decode_latin1_block(postfix, postfix_length, 0);

        // allocate buffer of suitable length
        char_t* buffer = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
        if (!buffer) return false;

        // second pass: convert latin1 input to utf8
        memcpy(buffer, data, prefix_length);

        uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
        uint8_t* oend = utf_decoder<utf8_writer>::decode_latin1_block(postfix, postfix_length, obegin + prefix_length);

        assert(oend == obegin + length);
        *oend = 0;

        out_buffer = buffer;
        out_length = length + 1;

        return true;
    }

    PUGI__FN bool convert_buffer(char_t*& out_buffer, size_t& out_length, xml_encoding encoding, const void* contents, size_t size, bool is_mutable)
    {
        // fast path: no conversion required
        if (encoding == encoding_utf8) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

        // source encoding is utf16
        if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
        {
            xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

            return (native_encoding == encoding) ?
                convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
                convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
        }

        // source encoding is utf32
        if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
        {
            xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

            return (native_encoding == encoding) ?
                convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
                convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
        }

        // source encoding is latin1
        if (encoding == encoding_latin1) return convert_buffer_latin1(out_buffer, out_length, contents, size, is_mutable);

        assert(!"Invalid encoding");
        return false;
    }
#endif

    PUGI__FN size_t as_utf8_begin(const wchar_t* str, size_t length)
    {
        // get length in utf8 characters
        return utf_decoder<utf8_counter>::decode_wchar_block(str, length, 0);
    }

    PUGI__FN void as_utf8_end(char* buffer, size_t size, const wchar_t* str, size_t length)
    {
        // convert to utf8
        uint8_t* begin = reinterpret_cast<uint8_t*>(buffer);
        uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block(str, length, begin);
    
        assert(begin + size == end);
        (void)!end;

        // zero-terminate
        buffer[size] = 0;
    }
    
#ifndef PUGIXML_NO_STL
    PUGI__FN std::string as_utf8_impl(const wchar_t* str, size_t length)
    {
        // first pass: get length in utf8 characters
        size_t size = as_utf8_begin(str, length);

        // allocate resulting string
        std::string result;
        result.resize(size);

        // second pass: convert to utf8
        if (size > 0) as_utf8_end(&result[0], size, str, length);

        return result;
    }

    PUGI__FN std::basic_string<wchar_t> as_wide_impl(const char* str, size_t size)
    {
        const uint8_t* data = reinterpret_cast<const uint8_t*>(str);

        // first pass: get length in wchar_t units
        size_t length = utf_decoder<wchar_counter>::decode_utf8_block(data, size, 0);

        // allocate resulting string
        std::basic_string<wchar_t> result;
        result.resize(length);

        // second pass: convert to wchar_t
        if (length > 0)
        {
            wchar_writer::value_type begin = reinterpret_cast<wchar_writer::value_type>(&result[0]);
            wchar_writer::value_type end = utf_decoder<wchar_writer>::decode_utf8_block(data, size, begin);

            assert(begin + length == end);
            (void)!end;
        }

        return result;
    }
#endif

    inline bool strcpy_insitu_allow(size_t length, uintptr_t header, uintptr_t header_mask, char_t* target)
    {
        // never reuse shared memory
        if (header & xml_memory_page_contents_shared_mask) return false;

        size_t target_length = strlength(target);

        // always reuse document buffer memory if possible
        if ((header & header_mask) == 0) return target_length >= length;

        // reuse heap memory if waste is not too great
        const size_t reuse_threshold = 32;

        return target_length >= length && (target_length < reuse_threshold || target_length - length < target_length / 2);
    }

    PUGI__FN bool strcpy_insitu(char_t*& dest, uintptr_t& header, uintptr_t header_mask, const char_t* source)
    {
        assert(header);

        size_t source_length = strlength(source);

        if (source_length == 0)
        {
            // empty string and null pointer are equivalent, so just deallocate old memory
            xml_allocator* alloc = reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask)->allocator;

            if (header & header_mask) alloc->deallocate_string(dest);
            
            // mark the string as not allocated
            dest = 0;
            header &= ~header_mask;

            return true;
        }
        else if (dest && strcpy_insitu_allow(source_length, header, header_mask, dest))
        {
            // we can reuse old buffer, so just copy the new data (including zero terminator)
            memcpy(dest, source, (source_length + 1) * sizeof(char_t));
            
            return true;
        }
        else
        {
            xml_allocator* alloc = reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask)->allocator;

            // allocate new buffer
            char_t* buf = alloc->allocate_string(source_length + 1);
            if (!buf) return false;

            // copy the string (including zero terminator)
            memcpy(buf, source, (source_length + 1) * sizeof(char_t));

            // deallocate old buffer (*after* the above to protect against overlapping memory and/or allocation failures)
            if (header & header_mask) alloc->deallocate_string(dest);
            
            // the string is now allocated, so set the flag
            dest = buf;
            header |= header_mask;

            return true;
        }
    }

    struct gap
    {
        char_t* end;
        size_t size;
            
        gap(): end(0), size(0)
        {
        }
            
        // Push new gap, move s count bytes further (skipping the gap).
        // apse previous gap.
        void push(char_t*& s, size_t count)
        {
            if (end) // there was a gap already; collapse it
            {
                // Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
                assert(s >= end);
                memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));
            }
                
            s += count; // end of current gap
                
            // "merge" two gaps
            end = s;
            size += count;
        }
            
        // Collapse all gaps, return past-the-end pointer
        char_t* flush(char_t* s)
        {
            if (end)
            {
                // Move [old_gap_end, current_pos) to [old_gap_start, ...)
                assert(s >= end);
                memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));

                return s - size;
            }
            else return s;
        }
    };
    
    PUGI__FN char_t* strconv_escape(char_t* s, gap& g)
    {
        char_t* stre = s + 1;

        switch (*stre)
        {
            case '#':    // &#...
            {
                unsigned int ucsc = 0;

                if (stre[1] == 'x') // &#x... (hex code)
                {
                    stre += 2;

                    char_t ch = *stre;

                    if (ch == ';') return stre;

                    for (;;)
                    {
                        if (static_cast<unsigned int>(ch - '0') <= 9)
                            ucsc = 16 * ucsc + (ch - '0');
                        else if (static_cast<unsigned int>((ch | ' ') - 'a') <= 5)
                            ucsc = 16 * ucsc + ((ch | ' ') - 'a' + 10);
                        else if (ch == ';')
                            break;
                        else // cancel
                            return stre;

                        ch = *++stre;
                    }
                    
                    ++stre;
                }
                else    // &#... (dec code)
                {
                    char_t ch = *++stre;

                    if (ch == ';') return stre;

                    for (;;)
                    {
                        if (static_cast<unsigned int>(static_cast<unsigned int>(ch) - '0') <= 9)
                            ucsc = 10 * ucsc + (ch - '0');
                        else if (ch == ';')
                            break;
                        else // cancel
                            return stre;

                        ch = *++stre;
                    }
                    
                    ++stre;
                }

            #ifdef PUGIXML_WCHAR_MODE
                s = reinterpret_cast<char_t*>(wchar_writer::any(reinterpret_cast<wchar_writer::value_type>(s), ucsc));
            #else
                s = reinterpret_cast<char_t*>(utf8_writer::any(reinterpret_cast<uint8_t*>(s), ucsc));
            #endif
                    
                g.push(s, stre - s);
                return stre;
            }

            case 'a':    // &a
            {
                ++stre;

                if (*stre == 'm') // &am
                {
                    if (*++stre == 'p' && *++stre == ';') // &amp;
                    {
                        *s++ = '&';
                        ++stre;
                            
                        g.push(s, stre - s);
                        return stre;
                    }
                }
                else if (*stre == 'p') // &ap
                {
                    if (*++stre == 'o' && *++stre == 's' && *++stre == ';') // &apos;
                    {
                        *s++ = '\'';
                        ++stre;

                        g.push(s, stre - s);
                        return stre;
                    }
                }
                break;
            }

            case 'g': // &g
            {
                if (*++stre == 't' && *++stre == ';') // &gt;
                {
                    *s++ = '>';
                    ++stre;
                    
                    g.push(s, stre - s);
                    return stre;
                }
                break;
            }

            case 'l': // &l
            {
                if (*++stre == 't' && *++stre == ';') // &lt;
                {
                    *s++ = '<';
                    ++stre;
                        
                    g.push(s, stre - s);
                    return stre;
                }
                break;
            }

            case 'q': // &q
            {
                if (*++stre == 'u' && *++stre == 'o' && *++stre == 't' && *++stre == ';') // &quot;
                {
                    *s++ = '"';
                    ++stre;
                    
                    g.push(s, stre - s);
                    return stre;
                }
                break;
            }

            default:
                break;
        }
        
        return stre;
    }

    // Parser utilities
    #define PUGI__ENDSWITH(c, e)        ((c) == (e) || ((c) == 0 && endch == (e)))
    #define PUGI__SKIPWS()              { while (PUGI__IS_CHARTYPE(*s, ct_space)) ++s; }
    #define PUGI__OPTSET(OPT)           ( optmsk & (OPT) )
    #define PUGI__PUSHNODE(TYPE)        { cursor = append_new_node(cursor, alloc, TYPE); if (!cursor) PUGI__THROW_ERROR(status_out_of_memory, s); }
    #define PUGI__POPNODE()             { cursor = cursor->parent; }
    #define PUGI__SCANFOR(X)            { while (*s != 0 && !(X)) ++s; }
    #define PUGI__SCANWHILE(X)          { while (X) ++s; }
    #define PUGI__SCANWHILE_UNROLL(X)   { for (;;) { char_t ss = s[0]; if (PUGI__UNLIKELY(!(X))) { break; } ss = s[1]; if (PUGI__UNLIKELY(!(X))) { s += 1; break; } ss = s[2]; if (PUGI__UNLIKELY(!(X))) { s += 2; break; } ss = s[3]; if (PUGI__UNLIKELY(!(X))) { s += 3; break; } s += 4; } }
    #define PUGI__ENDSEG()              { ch = *s; *s = 0; ++s; }
    #define PUGI__THROW_ERROR(err, m)   return error_offset = m, error_status = err, static_cast<char_t*>(0)
    #define PUGI__CHECK_ERROR(err, m)   { if (*s == 0) PUGI__THROW_ERROR(err, m); }

    PUGI__FN char_t* strconv_comment(char_t* s, char_t endch)
    {
        gap g;
        
        while (true)
        {
            PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_comment));
        
            if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
            {
                *s++ = '\n'; // replace first one with 0x0a
                
                if (*s == '\n') g.push(s, 1);
            }
            else if (s[0] == '-' && s[1] == '-' && PUGI__ENDSWITH(s[2], '>')) // comment ends here
            {
                *g.flush(s) = 0;
                
                return s + (s[2] == '>' ? 3 : 2);
            }
            else if (*s == 0)
            {
                return 0;
            }
            else ++s;
        }
    }

    PUGI__FN char_t* strconv_cdata(char_t* s, char_t endch)
    {
        gap g;
            
        while (true)
        {
            PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_cdata));
            
            if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
            {
                *s++ = '\n'; // replace first one with 0x0a
                
                if (*s == '\n') g.push(s, 1);
            }
            else if (s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>')) // CDATA ends here
            {
                *g.flush(s) = 0;
                
                return s + 1;
            }
            else if (*s == 0)
            {
                return 0;
            }
            else ++s;
        }
    }
    
    typedef char_t* (*strconv_pcdata_t)(char_t*);
        
    template <typename opt_trim, typename opt_eol, typename opt_escape> struct strconv_pcdata_impl
    {
        static char_t* parse(char_t* s)
        {
            gap g;

            char_t* begin = s;

            while (true)
            {
                PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_pcdata));

                if (*s == '<') // PCDATA ends here
                {
                    char_t* end = g.flush(s);

                    if (opt_trim::value)
                        while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
                            --end;

                    *end = 0;
                    
                    return s + 1;
                }
                else if (opt_eol::value && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
                {
                    *s++ = '\n'; // replace first one with 0x0a
                    
                    if (*s == '\n') g.push(s, 1);
                }
                else if (opt_escape::value && *s == '&')
                {
                    s = strconv_escape(s, g);
                }
                else if (*s == 0)
                {
                    char_t* end = g.flush(s);

                    if (opt_trim::value)
                        while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
                            --end;

                    *end = 0;

                    return s;
                }
                else ++s;
            }
        }
    };
    
    PUGI__FN strconv_pcdata_t get_strconv_pcdata(unsigned int optmask)
    {
        PUGI__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_trim_pcdata == 0x0800);

        switch (((optmask >> 4) & 3) | ((optmask >> 9) & 4)) // get bitmask for flags (eol escapes trim)
        {
        case 0: return strconv_pcdata_impl<opt_false, opt_false, opt_false>::parse;
        case 1: return strconv_pcdata_impl<opt_false, opt_false, opt_true>::parse;
        case 2: return strconv_pcdata_impl<opt_false, opt_true, opt_false>::parse;
        case 3: return strconv_pcdata_impl<opt_false, opt_true, opt_true>::parse;
        case 4: return strconv_pcdata_impl<opt_true, opt_false, opt_false>::parse;
        case 5: return strconv_pcdata_impl<opt_true, opt_false, opt_true>::parse;
        case 6: return strconv_pcdata_impl<opt_true, opt_true, opt_false>::parse;
        case 7: return strconv_pcdata_impl<opt_true, opt_true, opt_true>::parse;
        default: assert(false); return 0; // should not get here
        }
    }

    typedef char_t* (*strconv_attribute_t)(char_t*, char_t);
    
    template <typename opt_escape> struct strconv_attribute_impl
    {
        static char_t* parse_wnorm(char_t* s, char_t end_quote)
        {
            gap g;

            // trim leading whitespaces
            if (PUGI__IS_CHARTYPE(*s, ct_space))
            {
                char_t* str = s;
                
                do ++str;
                while (PUGI__IS_CHARTYPE(*str, ct_space));
                
                g.push(s, str - s);
            }

            while (true)
            {
                PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr_ws | ct_space));
                
                if (*s == end_quote)
                {
                    char_t* str = g.flush(s);
                    
                    do *str-- = 0;
                    while (PUGI__IS_CHARTYPE(*str, ct_space));
                
                    return s + 1;
                }
                else if (PUGI__IS_CHARTYPE(*s, ct_space))
                {
                    *s++ = ' ';
        
                    if (PUGI__IS_CHARTYPE(*s, ct_space))
                    {
                        char_t* str = s + 1;
                        while (PUGI__IS_CHARTYPE(*str, ct_space)) ++str;
                        
                        g.push(s, str - s);
                    }
                }
                else if (opt_escape::value && *s == '&')
                {
                    s = strconv_escape(s, g);
                }
                else if (!*s)
                {
                    return 0;
                }
                else ++s;
            }
        }

        static char_t* parse_wconv(char_t* s, char_t end_quote)
        {
            gap g;

            while (true)
            {
                PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr_ws));
                
                if (*s == end_quote)
                {
                    *g.flush(s) = 0;
                
                    return s + 1;
                }
                else if (PUGI__IS_CHARTYPE(*s, ct_space))
                {
                    if (*s == '\r')
                    {
                        *s++ = ' ';
                
                        if (*s == '\n') g.push(s, 1);
                    }
                    else *s++ = ' ';
                }
                else if (opt_escape::value && *s == '&')
                {
                    s = strconv_escape(s, g);
                }
                else if (!*s)
                {
                    return 0;
                }
                else ++s;
            }
        }

        static char_t* parse_eol(char_t* s, char_t end_quote)
        {
            gap g;

            while (true)
            {
                PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr));
                
                if (*s == end_quote)
                {
                    *g.flush(s) = 0;
                
                    return s + 1;
                }
                else if (*s == '\r')
                {
                    *s++ = '\n';
                    
                    if (*s == '\n') g.push(s, 1);
                }
                else if (opt_escape::value && *s == '&')
                {
                    s = strconv_escape(s, g);
                }
                else if (!*s)
                {
                    return 0;
                }
                else ++s;
            }
        }

        static char_t* parse_simple(char_t* s, char_t end_quote)
        {
            gap g;

            while (true)
            {
                PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPE(ss, ct_parse_attr));
                
                if (*s == end_quote)
                {
                    *g.flush(s) = 0;
                
                    return s + 1;
                }
                else if (opt_escape::value && *s == '&')
                {
                    s = strconv_escape(s, g);
                }
                else if (!*s)
                {
                    return 0;
                }
                else ++s;
            }
        }
    };

    PUGI__FN strconv_attribute_t get_strconv_attribute(unsigned int optmask)
    {
        PUGI__STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_wconv_attribute == 0x40 && parse_wnorm_attribute == 0x80);
        
        switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
        {
        case 0:  return strconv_attribute_impl<opt_false>::parse_simple;
        case 1:  return strconv_attribute_impl<opt_true>::parse_simple;
        case 2:  return strconv_attribute_impl<opt_false>::parse_eol;
        case 3:  return strconv_attribute_impl<opt_true>::parse_eol;
        case 4:  return strconv_attribute_impl<opt_false>::parse_wconv;
        case 5:  return strconv_attribute_impl<opt_true>::parse_wconv;
        case 6:  return strconv_attribute_impl<opt_false>::parse_wconv;
        case 7:  return strconv_attribute_impl<opt_true>::parse_wconv;
        case 8:  return strconv_attribute_impl<opt_false>::parse_wnorm;
        case 9:  return strconv_attribute_impl<opt_true>::parse_wnorm;
        case 10: return strconv_attribute_impl<opt_false>::parse_wnorm;
        case 11: return strconv_attribute_impl<opt_true>::parse_wnorm;
        case 12: return strconv_attribute_impl<opt_false>::parse_wnorm;
        case 13: return strconv_attribute_impl<opt_true>::parse_wnorm;
        case 14: return strconv_attribute_impl<opt_false>::parse_wnorm;
        case 15: return strconv_attribute_impl<opt_true>::parse_wnorm;
        default: assert(false); return 0; // should not get here
        }
    }

    inline xml_parse_result make_parse_result(xml_parse_status status, ptrdiff_t offset = 0)
    {
        xml_parse_result result;
        result.status = status;
        result.offset = offset;

        return result;
    }

    struct xml_parser
    {
        xml_allocator alloc;
        char_t* error_offset;
        xml_parse_status error_status;
        
        xml_parser(const xml_allocator& alloc_): alloc(alloc_), error_offset(0), error_status(status_ok)
        {
        }

        // DOCTYPE consists of nested sections of the following possible types:
        // <!-- ... -->, <? ... ?>, "...", '...'
        // <![...]]>
        // <!...>
        // First group can not contain nested groups
        // Second group can contain nested groups of the same type
        // Third group can contain all other groups
        char_t* parse_doctype_primitive(char_t* s)
        {
            if (*s == '"' || *s == '\'')
            {
                // quoted string
                char_t ch = *s++;
                PUGI__SCANFOR(*s == ch);
                if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

                s++;
            }
            else if (s[0] == '<' && s[1] == '?')
            {
                // <? ... ?>
                s += 2;
                PUGI__SCANFOR(s[0] == '?' && s[1] == '>'); // no need for ENDSWITH because ?> can't terminate proper doctype
                if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

                s += 2;
            }
            else if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-')
            {
                s += 4;
                PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && s[2] == '>'); // no need for ENDSWITH because --> can't terminate proper doctype
                if (!*s) PUGI__THROW_ERROR(status_bad_doctype, s);

                s += 3;
            }
            else PUGI__THROW_ERROR(status_bad_doctype, s);

            return s;
        }

        char_t* parse_doctype_ignore(char_t* s)
        {
            size_t depth = 0;

            assert(s[0] == '<' && s[1] == '!' && s[2] == '[');
            s += 3;

            while (*s)
            {
                if (s[0] == '<' && s[1] == '!' && s[2] == '[')
                {
                    // nested ignore section
                    s += 3;
                    depth++;
                }
                else if (s[0] == ']' && s[1] == ']' && s[2] == '>')
                {
                    // ignore section end
                    s += 3;

                    if (depth == 0)
                        return s;

                    depth--;
                }
                else s++;
            }

            PUGI__THROW_ERROR(status_bad_doctype, s);
        }

        char_t* parse_doctype_group(char_t* s, char_t endch)
        {
            size_t depth = 0;

            assert((s[0] == '<' || s[0] == 0) && s[1] == '!');
            s += 2;

            while (*s)
            {
                if (s[0] == '<' && s[1] == '!' && s[2] != '-')
                {
                    if (s[2] == '[')
                    {
                        // ignore
                        s = parse_doctype_ignore(s);
                        if (!s) return s;
                    }
                    else
                    {
                        // some control group
                        s += 2;
                        depth++;
                    }
                }
                else if (s[0] == '<' || s[0] == '"' || s[0] == '\'')
                {
                    // unknown tag (forbidden), or some primitive group
                    s = parse_doctype_primitive(s);
                    if (!s) return s;
                }
                else if (*s == '>')
                {
                    if (depth == 0)
                        return s;

                    depth--;
                    s++;
                }
                else s++;
            }

            if (depth != 0 || endch != '>') PUGI__THROW_ERROR(status_bad_doctype, s);

            return s;
        }

        char_t* parse_exclamation(char_t* s, xml_node_struct* cursor, unsigned int optmsk, char_t endch)
        {
            // parse node contents, starting with exclamation mark
            ++s;

            if (*s == '-') // '<!-...'
            {
                ++s;

                if (*s == '-') // '<!--...'
                {
                    ++s;

                    if (PUGI__OPTSET(parse_comments))
                    {
                        PUGI__PUSHNODE(node_comment); // Append a new node on the tree.
                        cursor->value = s; // Save the offset.
                    }

                    if (PUGI__OPTSET(parse_eol) && PUGI__OPTSET(parse_comments))
                    {
                        s = strconv_comment(s, endch);

                        if (!s) PUGI__THROW_ERROR(status_bad_comment, cursor->value);
                    }
                    else
                    {
                        // Scan for terminating '-->'.
                        PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && PUGI__ENDSWITH(s[2], '>'));
                        PUGI__CHECK_ERROR(status_bad_comment, s);

                        if (PUGI__OPTSET(parse_comments))
                            *s = 0; // Zero-terminate this segment at the first terminating '-'.

                        s += (s[2] == '>' ? 3 : 2); // Step over the '\0->'.
                    }
                }
                else PUGI__THROW_ERROR(status_bad_comment, s);
            }
            else if (*s == '[')
            {
                // '<![CDATA[...'
                if (*++s=='C' && *++s=='D' && *++s=='A' && *++s=='T' && *++s=='A' && *++s == '[')
                {
                    ++s;

                    if (PUGI__OPTSET(parse_cdata))
                    {
                        PUGI__PUSHNODE(node_cdata); // Append a new node on the tree.
                        cursor->value = s; // Save the offset.

                        if (PUGI__OPTSET(parse_eol))
                        {
                            s = strconv_cdata(s, endch);

                            if (!s) PUGI__THROW_ERROR(status_bad_cdata, cursor->value);
                        }
                        else
                        {
                            // Scan for terminating ']]>'.
                            PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>'));
                            PUGI__CHECK_ERROR(status_bad_cdata, s);

                            *s++ = 0; // Zero-terminate this segment.
                        }
                    }
                    else // Flagged for discard, but we still have to scan for the terminator.
                    {
                        // Scan for terminating ']]>'.
                        PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && PUGI__ENDSWITH(s[2], '>'));
                        PUGI__CHECK_ERROR(status_bad_cdata, s);

                        ++s;
                    }

                    s += (s[1] == '>' ? 2 : 1); // Step over the last ']>'.
                }
                else PUGI__THROW_ERROR(status_bad_cdata, s);
            }
            else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'C' && s[3] == 'T' && s[4] == 'Y' && s[5] == 'P' && PUGI__ENDSWITH(s[6], 'E'))
            {
                s -= 2;

                if (cursor->parent) PUGI__THROW_ERROR(status_bad_doctype, s);

                char_t* mark = s + 9;

                s = parse_doctype_group(s, endch);
                if (!s) return s;

                assert((*s == 0 && endch == '>') || *s == '>');
                if (*s) *s++ = 0;

                if (PUGI__OPTSET(parse_doctype))
                {
                    while (PUGI__IS_CHARTYPE(*mark, ct_space)) ++mark;

                    PUGI__PUSHNODE(node_doctype);

                    cursor->value = mark;
                }
            }
            else if (*s == 0 && endch == '-') PUGI__THROW_ERROR(status_bad_comment, s);
            else if (*s == 0 && endch == '[') PUGI__THROW_ERROR(status_bad_cdata, s);
            else PUGI__THROW_ERROR(status_unrecognized_tag, s);

            return s;
        }

        char_t* parse_question(char_t* s, xml_node_struct*& ref_cursor, unsigned int optmsk, char_t endch)
        {
            // load into registers
            xml_node_struct* cursor = ref_cursor;
            char_t ch = 0;

            // parse node contents, starting with question mark
            ++s;

            // read PI target
            char_t* target = s;

            if (!PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(status_bad_pi, s);

            PUGI__SCANWHILE(PUGI__IS_CHARTYPE(*s, ct_symbol));
            PUGI__CHECK_ERROR(status_bad_pi, s);

            // determine node type; stricmp / strcasecmp is not portable
            bool declaration = (target[0] | ' ') == 'x' && (target[1] | ' ') == 'm' && (target[2] | ' ') == 'l' && target + 3 == s;

            if (declaration ? PUGI__OPTSET(parse_declaration) : PUGI__OPTSET(parse_pi))
            {
                if (declaration)
                {
                    // disallow non top-level declarations
                    if (cursor->parent) PUGI__THROW_ERROR(status_bad_pi, s);

                    PUGI__PUSHNODE(node_declaration);
                }
                else
                {
                    PUGI__PUSHNODE(node_pi);
                }

                cursor->name = target;

                PUGI__ENDSEG();

                // parse value/attributes
                if (ch == '?')
                {
                    // empty node
                    if (!PUGI__ENDSWITH(*s, '>')) PUGI__THROW_ERROR(status_bad_pi, s);
                    s += (*s == '>');

                    PUGI__POPNODE();
                }
                else if (PUGI__IS_CHARTYPE(ch, ct_space))
                {
                    PUGI__SKIPWS();

                    // scan for tag end
                    char_t* value = s;

                    PUGI__SCANFOR(s[0] == '?' && PUGI__ENDSWITH(s[1], '>'));
                    PUGI__CHECK_ERROR(status_bad_pi, s);

                    if (declaration)
                    {
                        // replace ending ? with / so that 'element' terminates properly
                        *s = '/';

                        // we exit from this function with cursor at node_declaration, which is a signal to parse() to go to LOC_ATTRIBUTES
                        s = value;
                    }
                    else
                    {
                        // store value and step over >
                        cursor->value = value;
                        PUGI__POPNODE();

                        PUGI__ENDSEG();

                        s += (*s == '>');
                    }
                }
                else PUGI__THROW_ERROR(status_bad_pi, s);
            }
            else
            {
                // scan for tag end
                PUGI__SCANFOR(s[0] == '?' && PUGI__ENDSWITH(s[1], '>'));
                PUGI__CHECK_ERROR(status_bad_pi, s);

                s += (s[1] == '>' ? 2 : 1);
            }

            // store from registers
            ref_cursor = cursor;

            return s;
        }

        char_t* parse_tree(char_t* s, xml_node_struct* root, unsigned int optmsk, char_t endch)
        {
            strconv_attribute_t strconv_attribute = get_strconv_attribute(optmsk);
            strconv_pcdata_t strconv_pcdata = get_strconv_pcdata(optmsk);
            
            char_t ch = 0;
            xml_node_struct* cursor = root;
            char_t* mark = s;

            while (*s != 0)
            {
                if (*s == '<')
                {
                    ++s;

                LOC_TAG:
                    if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // '<#...'
                    {
                        PUGI__PUSHNODE(node_element); // Append a new node to the tree.

                        cursor->name = s;

                        PUGI__SCANWHILE_UNROLL(PUGI__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.
                        PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.

                        if (ch == '>')
                        {
                            // end of tag
                        }
                        else if (PUGI__IS_CHARTYPE(ch, ct_space))
                        {
                        LOC_ATTRIBUTES:
                            while (true)
                            {
                                PUGI__SKIPWS(); // Eat any whitespace.
                        
                                if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // <... #...
                                {
                                    xml_attribute_struct* a = append_new_attribute(cursor, alloc); // Make space for this attribute.
                                    if (!a) PUGI__THROW_ERROR(status_out_of_memory, s);

                                    a->name = s; // Save the offset.

                                    PUGI__SCANWHILE_UNROLL(PUGI__IS_CHARTYPE(ss, ct_symbol)); // Scan for a terminator.
                                    PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.

                                    if (PUGI__IS_CHARTYPE(ch, ct_space))
                                    {
                                        PUGI__SKIPWS(); // Eat any whitespace.

                                        ch = *s;
                                        ++s;
                                    }
                                    
                                    if (ch == '=') // '<... #=...'
                                    {
                                        PUGI__SKIPWS(); // Eat any whitespace.

                                        if (*s == '"' || *s == '\'') // '<... #="...'
                                        {
                                            ch = *s; // Save quote char to avoid breaking on "''" -or- '""'.
                                            ++s; // Step over the quote.
                                            a->value = s; // Save the offset.

                                            s = strconv_attribute(s, ch);
                                        
                                            if (!s) PUGI__THROW_ERROR(status_bad_attribute, a->value);

                                            // After this line the loop continues from the start;
                                            // Whitespaces, / and > are ok, symbols and EOF are wrong,
                                            // everything else will be detected
                                            if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(status_bad_attribute, s);
                                        }
                                        else PUGI__THROW_ERROR(status_bad_attribute, s);
                                    }
                                    else PUGI__THROW_ERROR(status_bad_attribute, s);
                                }
                                else if (*s == '/')
                                {
                                    ++s;
                                    
                                    if (*s == '>')
                                    {
                                        PUGI__POPNODE();
                                        s++;
                                        break;
                                    }
                                    else if (*s == 0 && endch == '>')
                                    {
                                        PUGI__POPNODE();
                                        break;
                                    }
                                    else PUGI__THROW_ERROR(status_bad_start_element, s);
                                }
                                else if (*s == '>')
                                {
                                    ++s;

                                    break;
                                }
                                else if (*s == 0 && endch == '>')
                                {
                                    break;
                                }
                                else PUGI__THROW_ERROR(status_bad_start_element, s);
                            }

                            // !!!
                        }
                        else if (ch == '/') // '<#.../'
                        {
                            if (!PUGI__ENDSWITH(*s, '>')) PUGI__THROW_ERROR(status_bad_start_element, s);

                            PUGI__POPNODE(); // Pop.

                            s += (*s == '>');
                        }
                        else if (ch == 0)
                        {
                            // we stepped over null terminator, backtrack & handle closing tag
                            --s;
                            
                            if (endch != '>') PUGI__THROW_ERROR(status_bad_start_element, s);
                        }
                        else PUGI__THROW_ERROR(status_bad_start_element, s);
                    }
                    else if (*s == '/')
                    {
                        ++s;

                        char_t* name = cursor->name;
                        if (!name) PUGI__THROW_ERROR(status_end_element_mismatch, s);
                        
                        while (PUGI__IS_CHARTYPE(*s, ct_symbol))
                        {
                            if (*s++ != *name++) PUGI__THROW_ERROR(status_end_element_mismatch, s);
                        }

                        if (*name)
                        {
                            if (*s == 0 && name[0] == endch && name[1] == 0) PUGI__THROW_ERROR(status_bad_end_element, s);
                            else PUGI__THROW_ERROR(status_end_element_mismatch, s);
                        }
                            
                        PUGI__POPNODE(); // Pop.

                        PUGI__SKIPWS();

                        if (*s == 0)
                        {
                            if (endch != '>') PUGI__THROW_ERROR(status_bad_end_element, s);
                        }
                        else
                        {
                            if (*s != '>') PUGI__THROW_ERROR(status_bad_end_element, s);
                            ++s;
                        }
                    }
                    else if (*s == '?') // '<?...'
                    {
                        s = parse_question(s, cursor, optmsk, endch);
                        if (!s) return s;

                        assert(cursor);
                        if (PUGI__NODETYPE(cursor) == node_declaration) goto LOC_ATTRIBUTES;
                    }
                    else if (*s == '!') // '<!...'
                    {
                        s = parse_exclamation(s, cursor, optmsk, endch);
                        if (!s) return s;
                    }
                    else if (*s == 0 && endch == '?') PUGI__THROW_ERROR(status_bad_pi, s);
                    else PUGI__THROW_ERROR(status_unrecognized_tag, s);
                }
                else
                {
                    mark = s; // Save this offset while searching for a terminator.

                    PUGI__SKIPWS(); // Eat whitespace if no genuine PCDATA here.

                    if (*s == '<' || !*s)
                    {
                        // We skipped some whitespace characters because otherwise we would take the tag branch instead of PCDATA one
                        assert(mark != s);

                        if (!PUGI__OPTSET(parse_ws_pcdata | parse_ws_pcdata_single) || PUGI__OPTSET(parse_trim_pcdata))
                        {
                            continue;
                        }
                        else if (PUGI__OPTSET(parse_ws_pcdata_single))
                        {
                            if (s[0] != '<' || s[1] != '/' || cursor->first_child) continue;
                        }
                    }

                    if (!PUGI__OPTSET(parse_trim_pcdata))
                        s = mark;
                            
                    if (cursor->parent || PUGI__OPTSET(parse_fragment))
                    {
                        PUGI__PUSHNODE(node_pcdata); // Append a new node on the tree.
                        cursor->value = s; // Save the offset.

                        s = strconv_pcdata(s);
                                
                        PUGI__POPNODE(); // Pop since this is a standalone.
                        
                        if (!*s) break;
                    }
                    else
                    {
                        PUGI__SCANFOR(*s == '<'); // '...<'
                        if (!*s) break;
                        
                        ++s;
                    }

                    // We're after '<'
                    goto LOC_TAG;
                }
            }

            // check that last tag is closed
            if (cursor != root) PUGI__THROW_ERROR(status_end_element_mismatch, s);

            return s;
        }

    #ifdef PUGIXML_WCHAR_MODE
        static char_t* parse_skip_bom(char_t* s)
        {
            unsigned int bom = 0xfeff;
            return (s[0] == static_cast<wchar_t>(bom)) ? s + 1 : s;
        }
    #else
        static char_t* parse_skip_bom(char_t* s)
        {
            return (s[0] == '\xef' && s[1] == '\xbb' && s[2] == '\xbf') ? s + 3 : s;
        }
    #endif

        static bool has_element_node_siblings(xml_node_struct* node)
        {
            while (node)
            {
                if (PUGI__NODETYPE(node) == node_element) return true;

                node = node->next_sibling;
            }

            return false;
        }

        static xml_parse_result parse(char_t* buffer, size_t length, xml_document_struct* xmldoc, xml_node_struct* root, unsigned int optmsk)
        {
            // allocator object is a part of document object
            xml_allocator& alloc_ = *static_cast<xml_allocator*>(xmldoc);

            // early-out for empty documents
            if (length == 0)
                return make_parse_result(PUGI__OPTSET(parse_fragment) ? status_ok : status_no_document_element);

            // get last child of the root before parsing
            xml_node_struct* last_root_child = root->first_child ? root->first_child->prev_sibling_c : 0;
    
            // create parser on stack
            xml_parser parser(alloc_);

            // save last character and make buffer zero-terminated (speeds up parsing)
            char_t endch = buffer[length - 1];
            buffer[length - 1] = 0;
            
            // skip BOM to make sure it does not end up as part of parse output
            char_t* buffer_data = parse_skip_bom(buffer);

            // perform actual parsing
            parser.parse_tree(buffer_data, root, optmsk, endch);

            // update allocator state
            alloc_ = parser.alloc;

            xml_parse_result result = make_parse_result(parser.error_status, parser.error_offset ? parser.error_offset - buffer : 0);
            assert(result.offset >= 0 && static_cast<size_t>(result.offset) <= length);

            if (result)
            {
                // since we removed last character, we have to handle the only possible false positive (stray <)
                if (endch == '<')
                    return make_parse_result(status_unrecognized_tag, length - 1);

                // check if there are any element nodes parsed
                xml_node_struct* first_root_child_parsed = last_root_child ? last_root_child->next_sibling : root->first_child;

                if (!PUGI__OPTSET(parse_fragment) && !has_element_node_siblings(first_root_child_parsed))
                    return make_parse_result(status_no_document_element, length - 1);
            }
            else
            {
                // roll back offset if it occurs on a null terminator in the source buffer
                if (result.offset > 0 && static_cast<size_t>(result.offset) == length - 1 && endch == 0)
                    result.offset--;
            }

            return result;
        }
    };

    // Output facilities
    PUGI__FN xml_encoding get_write_native_encoding()
    {
    #ifdef PUGIXML_WCHAR_MODE
        return get_wchar_encoding();
    #else
        return encoding_utf8;
    #endif
    }

    PUGI__FN xml_encoding get_write_encoding(xml_encoding encoding)
    {
        // replace wchar encoding with utf implementation
        if (encoding == encoding_wchar) return get_wchar_encoding();

        // replace utf16 encoding with utf16 with specific endianness
        if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

        // replace utf32 encoding with utf32 with specific endianness
        if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

        // only do autodetection if no explicit encoding is requested
        if (encoding != encoding_auto) return encoding;

        // assume utf8 encoding
        return encoding_utf8;
    }

#ifdef PUGIXML_WCHAR_MODE
    PUGI__FN size_t get_valid_length(const char_t* data, size_t length)
    {
        if (length < 1) return 0;

        // discard last character if it's the lead of a surrogate pair 
        return (sizeof(wchar_t) == 2 && static_cast<unsigned int>(static_cast<uint16_t>(data[length - 1]) - 0xD800) < 0x400) ? length - 1 : length;
    }

    PUGI__FN size_t convert_buffer_output(char_t* r_char, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, const char_t* data, size_t length, xml_encoding encoding)
    {
        // only endian-swapping is required
        if (need_endian_swap_utf(encoding, get_wchar_encoding()))
        {
            convert_wchar_endian_swap(r_char, data, length);

            return length * sizeof(char_t);
        }
    
        // convert to utf8
        if (encoding == encoding_utf8)
        {
            uint8_t* dest = r_u8;
            uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block(data, length, dest);

            return static_cast<size_t>(end - dest);
        }

        // convert to utf16
        if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
        {
            uint16_t* dest = r_u16;

            // convert to native utf16
            uint16_t* end = utf_decoder<utf16_writer>::decode_wchar_block(data, length, dest);

            // swap if necessary
            xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

            if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

            return static_cast<size_t>(end - dest) * sizeof(uint16_t);
        }

        // convert to utf32
        if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
        {
            uint32_t* dest = r_u32;

            // convert to native utf32
            uint32_t* end = utf_decoder<utf32_writer>::decode_wchar_block(data, length, dest);

            // swap if necessary
            xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

            if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

            return static_cast<size_t>(end - dest) * sizeof(uint32_t);
        }

        // convert to latin1
        if (encoding == encoding_latin1)
        {
            uint8_t* dest = r_u8;
            uint8_t* end = utf_decoder<latin1_writer>::decode_wchar_block(data, length, dest);

            return static_cast<size_t>(end - dest);
        }

        assert(!"Invalid encoding");
        return 0;
    }
#else
    PUGI__FN size_t get_valid_length(const char_t* data, size_t length)
    {
        if (length < 5) return 0;

        for (size_t i = 1; i <= 4; ++i)
        {
            uint8_t ch = static_cast<uint8_t>(data[length - i]);

            // either a standalone character or a leading one
            if ((ch & 0xc0) != 0x80) return length - i;
        }

        // there are four non-leading characters at the end, sequence tail is broken so might as well process the whole chunk
        return length;
    }

    PUGI__FN size_t convert_buffer_output(char_t* /* r_char */, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, const char_t* data, size_t length, xml_encoding encoding)
    {
        if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
        {
            uint16_t* dest = r_u16;

            // convert to native utf16
            uint16_t* end = utf_decoder<utf16_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

            // swap if necessary
            xml_encoding native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

            if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

            return static_cast<size_t>(end - dest) * sizeof(uint16_t);
        }

        if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
        {
            uint32_t* dest = r_u32;

            // convert to native utf32
            uint32_t* end = utf_decoder<utf32_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

            // swap if necessary
            xml_encoding native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

            if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

            return static_cast<size_t>(end - dest) * sizeof(uint32_t);
        }

        if (encoding == encoding_latin1)
        {
            uint8_t* dest = r_u8;
            uint8_t* end = utf_decoder<latin1_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

            return static_cast<size_t>(end - dest);
        }

        assert(!"Invalid encoding");
        return 0;
    }
#endif

    class xml_buffered_writer
    {
        xml_buffered_writer(const xml_buffered_writer&);
        xml_buffered_writer& operator=(const xml_buffered_writer&);

    public:
        xml_buffered_writer(xml_writer& writer_, xml_encoding user_encoding): writer(writer_), bufsize(0), encoding(get_write_encoding(user_encoding))
        {
            PUGI__STATIC_ASSERT(bufcapacity >= 8);
        }

        ~xml_buffered_writer()
        {
            flush();
        }

        size_t flush()
        {
            flush(buffer, bufsize);
            bufsize = 0;
            return 0;
        }

        void flush(const char_t* data, size_t size)
        {
            if (size == 0) return;

            // fast path, just write data
            if (encoding == get_write_native_encoding())
                writer.write(data, size * sizeof(char_t));
            else
            {
                // convert chunk
                size_t result = convert_buffer_output(scratch.data_char, scratch.data_u8, scratch.data_u16, scratch.data_u32, data, size, encoding);
                assert(result <= sizeof(scratch));

                // write data
                writer.write(scratch.data_u8, result);
            }
        }

        void write_direct(const char_t* data, size_t length)
        {
            // flush the remaining buffer contents
            flush();

            // handle large chunks
            if (length > bufcapacity)
            {
                if (encoding == get_write_native_encoding())
                {
                    // fast path, can just write data chunk
                    writer.write(data, length * sizeof(char_t));
                    return;
                }

                // need to convert in suitable chunks
                while (length > bufcapacity)
                {
                    // get chunk size by selecting such number of characters that are guaranteed to fit into scratch buffer
                    // and form a complete codepoint sequence (i.e. discard start of last codepoint if necessary)
                    size_t chunk_size = get_valid_length(data, bufcapacity);
                    assert(chunk_size);

                    // convert chunk and write
                    flush(data, chunk_size);

                    // iterate
                    data += chunk_size;
                    length -= chunk_size;
                }

                // small tail is copied below
                bufsize = 0;
            }

            memcpy(buffer + bufsize, data, length * sizeof(char_t));
            bufsize += length;
        }

        void write_buffer(const char_t* data, size_t length)
        {
            size_t offset = bufsize;

            if (offset + length <= bufcapacity)
            {
                memcpy(buffer + offset, data, length * sizeof(char_t));
                bufsize = offset + length;
            }
            else
            {
                write_direct(data, length);
            }
        }

        void write_string(const char_t* data)
        {
            // write the part of the string that fits in the buffer
            size_t offset = bufsize;

            while (*data && offset < bufcapacity)
                buffer[offset++] = *data++;

            // write the rest
            if (offset < bufcapacity)
            {
                bufsize = offset;
            }
            else
            {
                // backtrack a bit if we have split the codepoint
                size_t length = offset - bufsize;
                size_t extra = length - get_valid_length(data - length, length);

                bufsize = offset - extra;

                write_direct(data - extra, strlength(data) + extra);
            }
        }

        void write(char_t d0)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 1) offset = flush();

            buffer[offset + 0] = d0;
            bufsize = offset + 1;
        }

        void write(char_t d0, char_t d1)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 2) offset = flush();

            buffer[offset + 0] = d0;
            buffer[offset + 1] = d1;
            bufsize = offset + 2;
        }

        void write(char_t d0, char_t d1, char_t d2)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 3) offset = flush();

            buffer[offset + 0] = d0;
            buffer[offset + 1] = d1;
            buffer[offset + 2] = d2;
            bufsize = offset + 3;
        }

        void write(char_t d0, char_t d1, char_t d2, char_t d3)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 4) offset = flush();

            buffer[offset + 0] = d0;
            buffer[offset + 1] = d1;
            buffer[offset + 2] = d2;
            buffer[offset + 3] = d3;
            bufsize = offset + 4;
        }

        void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 5) offset = flush();

            buffer[offset + 0] = d0;
            buffer[offset + 1] = d1;
            buffer[offset + 2] = d2;
            buffer[offset + 3] = d3;
            buffer[offset + 4] = d4;
            bufsize = offset + 5;
        }

        void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4, char_t d5)
        {
            size_t offset = bufsize;
            if (offset > bufcapacity - 6) offset = flush();

            buffer[offset + 0] = d0;
            buffer[offset + 1] = d1;
            buffer[offset + 2] = d2;
            buffer[offset + 3] = d3;
            buffer[offset + 4] = d4;
            buffer[offset + 5] = d5;
            bufsize = offset + 6;
        }

        // utf8 maximum expansion: x4 (-> utf32)
        // utf16 maximum expansion: x2 (-> utf32)
        // utf32 maximum expansion: x1
        enum
        {
            bufcapacitybytes =
            #ifdef PUGIXML_MEMORY_OUTPUT_STACK
                PUGIXML_MEMORY_OUTPUT_STACK
            #else
                10240
            #endif
            ,
            bufcapacity = bufcapacitybytes / (sizeof(char_t) + 4)
        };

        char_t buffer[bufcapacity];

        union
        {
            uint8_t data_u8[4 * bufcapacity];
            uint16_t data_u16[2 * bufcapacity];
            uint32_t data_u32[bufcapacity];
            char_t data_char[bufcapacity];
        } scratch;

        xml_writer& writer;
        size_t bufsize;
        xml_encoding encoding;
    };

    PUGI__FN void text_output_escaped(xml_buffered_writer& writer, const char_t* s, chartypex_t type)
    {
        while (*s)
        {
            const char_t* prev = s;
            
            // While *s is a usual symbol
            PUGI__SCANWHILE_UNROLL(!PUGI__IS_CHARTYPEX(ss, type));
        
            writer.write_buffer(prev, static_cast<size_t>(s - prev));

            switch (*s)
            {
                case 0: break;
                case '&':
                    writer.write('&', 'a', 'm', 'p', ';');
                    ++s;
                    break;
                case '<':
                    writer.write('&', 'l', 't', ';');
                    ++s;
                    break;
                case '>':
                    writer.write('&', 'g', 't', ';');
                    ++s;
                    break;
                case '"':
                    writer.write('&', 'q', 'u', 'o', 't', ';');
                    ++s;
                    break;
                default: // s is not a usual symbol
                {
                    unsigned int ch = static_cast<unsigned int>(*s++);
                    assert(ch < 32);

                    writer.write('&', '#', static_cast<char_t>((ch / 10) + '0'), static_cast<char_t>((ch % 10) + '0'), ';');
                }
            }
        }
    }

    PUGI__FN void text_output(xml_buffered_writer& writer, const char_t* s, chartypex_t type, unsigned int flags)
    {
        if (flags & format_no_escapes)
            writer.write_string(s);
        else
            text_output_escaped(writer, s, type);
    }

    PUGI__FN void text_output_cdata(xml_buffered_writer& writer, const char_t* s)
    {
        do
        {
            writer.write('<', '!', '[', 'C', 'D');
            writer.write('A', 'T', 'A', '[');

            const char_t* prev = s;

            // look for ]]> sequence - we can't output it as is since it terminates CDATA
            while (*s && !(s[0] == ']' && s[1] == ']' && s[2] == '>')) ++s;

            // skip ]] if we stopped at ]]>, > will go to the next CDATA section
            if (*s) s += 2;

            writer.write_buffer(prev, static_cast<size_t>(s - prev));

            writer.write(']', ']', '>');
        }
        while (*s);
    }

    PUGI__FN void text_output_indent(xml_buffered_writer& writer, const char_t* indent, size_t indent_length, unsigned int depth)
    {
        switch (indent_length)
        {
        case 1:
        {
            for (unsigned int i = 0; i < depth; ++i)
                writer.write(indent[0]);
            break;
        }

        case 2:
        {
            for (unsigned int i = 0; i < depth; ++i)
                writer.write(indent[0], indent[1]);
            break;
        }

        case 3:
        {
            for (unsigned int i = 0; i < depth; ++i)
                writer.write(indent[0], indent[1], indent[2]);
            break;
        }

        case 4:
        {
            for (unsigned int i = 0; i < depth; ++i)
                writer.write(indent[0], indent[1], indent[2], indent[3]);
            break;
        }

        default:
        {
            for (unsigned int i = 0; i < depth; ++i)
                writer.write_buffer(indent, indent_length);
        }
        }
    }

    PUGI__FN void node_output_comment(xml_buffered_writer& writer, const char_t* s)
    {
        writer.write('<', '!', '-', '-');

        while (*s)
        {
            const char_t* prev = s;

            // look for -\0 or -- sequence - we can't output it since -- is illegal in comment body
            while (*s && !(s[0] == '-' && (s[1] == '-' || s[1] == 0))) ++s;

            writer.write_buffer(prev, static_cast<size_t>(s - prev));

            if (*s)
            {
                assert(*s == '-');

                writer.write('-', ' ');
                ++s;
            }
        }

        writer.write('-', '-', '>');
    }

    PUGI__FN void node_output_pi_value(xml_buffered_writer& writer, const char_t* s)
    {
        while (*s)
        {
            const char_t* prev = s;

            // look for ?> sequence - we can't output it since ?> terminates PI
            while (*s && !(s[0] == '?' && s[1] == '>')) ++s;

            writer.write_buffer(prev, static_cast<size_t>(s - prev));

            if (*s)
            {
                assert(s[0] == '?' && s[1] == '>');

                writer.write('?', ' ', '>');
                s += 2;
            }
        }
    }

    PUGI__FN void node_output_attributes(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
    {
        const char_t* default_name = PUGIXML_TEXT(":anonymous");

        for (xml_attribute_struct* a = node->first_attribute; a; a = a->next_attribute)
        {
            writer.write(' ');
            writer.write_string(a->name ? a->name : default_name);
            writer.write('=', '"');

            if (a->value)
                text_output(writer, a->value, ctx_special_attr, flags);

            writer.write('"');
        }
    }

    PUGI__FN bool node_output_start(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
    {
        const char_t* default_name = PUGIXML_TEXT(":anonymous");
        const char_t* name = node->name ? node->name : default_name;

        writer.write('<');
        writer.write_string(name);

        if (node->first_attribute)
            node_output_attributes(writer, node, flags);

        if (!node->first_child)
        {
            writer.write(' ', '/', '>');

            return false;
        }
        else
        {
            writer.write('>');

            return true;
        }
    }

    PUGI__FN void node_output_end(xml_buffered_writer& writer, xml_node_struct* node)
    {
        const char_t* default_name = PUGIXML_TEXT(":anonymous");
        const char_t* name = node->name ? node->name : default_name;

        writer.write('<', '/');
        writer.write_string(name);
        writer.write('>');
    }

    PUGI__FN void node_output_simple(xml_buffered_writer& writer, xml_node_struct* node, unsigned int flags)
    {
        const char_t* default_name = PUGIXML_TEXT(":anonymous");

        switch (PUGI__NODETYPE(node))
        {
            case node_pcdata:
                text_output(writer, node->value ? node->value : PUGIXML_TEXT(""), ctx_special_pcdata, flags);
                break;

            case node_cdata:
                text_output_cdata(writer, node->value ? node->value : PUGIXML_TEXT(""));
                break;

            case node_comment:
                node_output_comment(writer, node->value ? node->value : PUGIXML_TEXT(""));
                break;

            case node_pi:
                writer.write('<', '?');
                writer.write_string(node->name ? node->name : default_name);

                if (node->value)
                {
                    writer.write(' ');
                    node_output_pi_value(writer, node->value);
                }

                writer.write('?', '>');
                break;

            case node_declaration:
                writer.write('<', '?');
                writer.write_string(node->name ? node->name : default_name);
                node_output_attributes(writer, node, flags);
                writer.write('?', '>');
                break;

            case node_doctype:
                writer.write('<', '!', 'D', 'O', 'C');
                writer.write('T', 'Y', 'P', 'E');

                if (node->value)
                {
                    writer.write(' ');
                    writer.write_string(node->value);
                }

                writer.write('>');
                break;

            default:
                assert(!"Invalid node type");
        }
    }

    enum indent_flags_t
    {
        indent_newline = 1,
        indent_indent = 2
    };

    PUGI__FN void node_output(xml_buffered_writer& writer, xml_node_struct* root, const char_t* indent, unsigned int flags, unsigned int depth)
    {
        size_t indent_length = ((flags & (format_indent | format_raw)) == format_indent) ? strlength(indent) : 0;
        unsigned int indent_flags = indent_indent;

        xml_node_struct* node = root;

        do
        {
            assert(node);

            // begin writing current node
            if (PUGI__NODETYPE(node) == node_pcdata || PUGI__NODETYPE(node) == node_cdata)
            {
                node_output_simple(writer, node, flags);

                indent_flags = 0;
            }
            else
            {
                if ((indent_flags & indent_newline) && (flags & format_raw) == 0)
                    writer.write('\n');

                if ((indent_flags & indent_indent) && indent_length)
                    text_output_indent(writer, indent, indent_length, depth);

                if (PUGI__NODETYPE(node) == node_element)
                {
                    indent_flags = indent_newline | indent_indent;

                    if (node_output_start(writer, node, flags))
                    {
                        node = node->first_child;
                        depth++;
                        continue;
                    }
                }
                else if (PUGI__NODETYPE(node) == node_document)
                {
                    indent_flags = indent_indent;

                    if (node->first_child)
                    {
                        node = node->first_child;
                        continue;
                    }
                }
                else
                {
                    node_output_simple(writer, node, flags);

                    indent_flags = indent_newline | indent_indent;
                }
            }

            // continue to the next node
            while (node != root)
            {
                if (node->next_sibling)
                {
                    node = node->next_sibling;
                    break;
                }

                node = node->parent;

                // write closing node
                if (PUGI__NODETYPE(node) == node_element)
                {
                    depth--;

                    if ((indent_flags & indent_newline) && (flags & format_raw) == 0)
                        writer.write('\n');

                    if ((indent_flags & indent_indent) && indent_length)
                        text_output_indent(writer, indent, indent_length, depth);

                    node_output_end(writer, node);

                    indent_flags = indent_newline | indent_indent;
                }
            }
        }
        while (node != root);

        if ((indent_flags & indent_newline) && (flags & format_raw) == 0)
            writer.write('\n');
    }

    PUGI__FN bool has_declaration(xml_node_struct* node)
    {
        for (xml_node_struct* child = node->first_child; child; child = child->next_sibling)
        {
            xml_node_type type = PUGI__NODETYPE(child);

            if (type == node_declaration) return true;
            if (type == node_element) return false;
        }

        return false;
    }

    PUGI__FN bool is_attribute_of(xml_attribute_struct* attr, xml_node_struct* node)
    {
        for (xml_attribute_struct* a = node->first_attribute; a; a = a->next_attribute)
            if (a == attr)
                return true;

        return false;
    }

    PUGI__FN bool allow_insert_attribute(xml_node_type parent)
    {
        return parent == node_element || parent == node_declaration;
    }

    PUGI__FN bool allow_insert_child(xml_node_type parent, xml_node_type child)
    {
        if (parent != node_document && parent != node_element) return false;
        if (child == node_document || child == node_null) return false;
        if (parent != node_document && (child == node_declaration || child == node_doctype)) return false;

        return true;
    }

    PUGI__FN bool allow_move(xml_node parent, xml_node child)
    {
        // check that child can be a child of parent
        if (!allow_insert_child(parent.type(), child.type()))
            return false;

        // check that node is not moved between documents
        if (parent.root() != child.root())
            return false;

        // check that new parent is not in the child subtree
        xml_node cur = parent;

        while (cur)
        {
            if (cur == child)
                return false;

            cur = cur.parent();
        }

        return true;
    }

    PUGI__FN void node_copy_string(char_t*& dest, uintptr_t& header, uintptr_t header_mask, char_t* source, uintptr_t& source_header, xml_allocator* alloc)
    {
        assert(!dest && (header & header_mask) == 0);

        if (source)
        {
            if (alloc && (source_header & header_mask) == 0)
            {
                dest = source;

                // since strcpy_insitu can reuse document buffer memory we need to mark both source and dest as shared
                header |= xml_memory_page_contents_shared_mask;
                source_header |= xml_memory_page_contents_shared_mask;
            }
            else
                strcpy_insitu(dest, header, header_mask, source);
        }
    }

    PUGI__FN void node_copy_contents(xml_node_struct* dn, xml_node_struct* sn, xml_allocator* shared_alloc)
    {
        node_copy_string(dn->name, dn->header, xml_memory_page_name_allocated_mask, sn->name, sn->header, shared_alloc);
        node_copy_string(dn->value, dn->header, xml_memory_page_value_allocated_mask, sn->value, sn->header, shared_alloc);

        for (xml_attribute_struct* sa = sn->first_attribute; sa; sa = sa->next_attribute)
        {
            xml_attribute_struct* da = append_new_attribute(dn, get_allocator(dn));

            if (da)
            {
                node_copy_string(da->name, da->header, xml_memory_page_name_allocated_mask, sa->name, sa->header, shared_alloc);
                node_copy_string(da->value, da->header, xml_memory_page_value_allocated_mask, sa->value, sa->header, shared_alloc);
            }
        }
    }

    PUGI__FN void node_copy_tree(xml_node_struct* dn, xml_node_struct* sn)
    {
        xml_allocator& alloc = get_allocator(dn);
        xml_allocator* shared_alloc = (&alloc == &get_allocator(sn)) ? &alloc : 0;

        node_copy_contents(dn, sn, shared_alloc);

        xml_node_struct* dit = dn;
        xml_node_struct* sit = sn->first_child;

        while (sit && sit != sn)
        {
            if (sit != dn)
            {
                xml_node_struct* copy = append_new_node(dit, alloc, PUGI__NODETYPE(sit));

                if (copy)
                {
                    node_copy_contents(copy, sit, shared_alloc);

                    if (sit->first_child)
                    {
                        dit = copy;
                        sit = sit->first_child;
                        continue;
                    }
                }
            }

            // continue to the next node
            do
            {
                if (sit->next_sibling)
                {
                    sit = sit->next_sibling;
                    break;
                }

                sit = sit->parent;
                dit = dit->parent;
            }
            while (sit != sn);
        }
    }

    inline bool is_text_node(xml_node_struct* node)
    {
        xml_node_type type = PUGI__NODETYPE(node);

        return type == node_pcdata || type == node_cdata;
    }

    // get value with conversion functions
    PUGI__FN int get_integer_base(const char_t* value)
    {
        const char_t* s = value;

        while (PUGI__IS_CHARTYPE(*s, ct_space))
            s++;

        if (*s == '-')
            s++;

        return (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 16 : 10;
    }

    PUGI__FN int get_value_int(const char_t* value, int def)
    {
        if (!value) return def;

        int base = get_integer_base(value);

    #ifdef PUGIXML_WCHAR_MODE
        return static_cast<int>(wcstol(value, 0, base));
    #else
        return static_cast<int>(strtol(value, 0, base));
    #endif
    }

    PUGI__FN unsigned int get_value_uint(const char_t* value, unsigned int def)
    {
        if (!value) return def;

        int base = get_integer_base(value);

    #ifdef PUGIXML_WCHAR_MODE
        return static_cast<unsigned int>(wcstoul(value, 0, base));
    #else
        return static_cast<unsigned int>(strtoul(value, 0, base));
    #endif
    }

    PUGI__FN double get_value_double(const char_t* value, double def)
    {
        if (!value) return def;

    #ifdef PUGIXML_WCHAR_MODE
        return wcstod(value, 0);
    #else
        return strtod(value, 0);
    #endif
    }

    PUGI__FN float get_value_float(const char_t* value, float def)
    {
        if (!value) return def;

    #ifdef PUGIXML_WCHAR_MODE
        return static_cast<float>(wcstod(value, 0));
    #else
        return static_cast<float>(strtod(value, 0));
    #endif
    }

    PUGI__FN bool get_value_bool(const char_t* value, bool def)
    {
        if (!value) return def;

        // only look at first char
        char_t first = *value;

        // 1*, t* (true), T* (True), y* (yes), Y* (YES)
        return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN long long get_value_llong(const char_t* value, long long def)
    {
        if (!value) return def;

        int base = get_integer_base(value);

    #ifdef PUGIXML_WCHAR_MODE
        #ifdef PUGI__MSVC_CRT_VERSION
            return _wcstoi64(value, 0, base);
        #else
            return wcstoll(value, 0, base);
        #endif
    #else
        #ifdef PUGI__MSVC_CRT_VERSION
            return _strtoi64(value, 0, base);
        #else
            return strtoll(value, 0, base);
        #endif
    #endif
    }

    PUGI__FN unsigned long long get_value_ullong(const char_t* value, unsigned long long def)
    {
        if (!value) return def;

        int base = get_integer_base(value);

    #ifdef PUGIXML_WCHAR_MODE
        #ifdef PUGI__MSVC_CRT_VERSION
            return _wcstoui64(value, 0, base);
        #else
            return wcstoull(value, 0, base);
        #endif
    #else
        #ifdef PUGI__MSVC_CRT_VERSION
            return _strtoui64(value, 0, base);
        #else
            return strtoull(value, 0, base);
        #endif
    #endif
    }
#endif

    // set value with conversion functions
    PUGI__FN bool set_value_buffer(char_t*& dest, uintptr_t& header, uintptr_t header_mask, char (&buf)[128])
    {
    #ifdef PUGIXML_WCHAR_MODE
        char_t wbuf[128];
        impl::widen_ascii(wbuf, buf);

        return strcpy_insitu(dest, header, header_mask, wbuf);
    #else
        return strcpy_insitu(dest, header, header_mask, buf);
    #endif
    }

    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, int value)
    {
        char buf[128];
        sprintf(buf, "%d", value);
    
        return set_value_buffer(dest, header, header_mask, buf);
    }

    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, unsigned int value)
    {
        char buf[128];
        sprintf(buf, "%u", value);

        return set_value_buffer(dest, header, header_mask, buf);
    }

    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, float value)
    {
        char buf[128];
        sprintf(buf, "%.9g", value);

        return set_value_buffer(dest, header, header_mask, buf);
    }
    
    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, double value)
    {
        char buf[128];
        sprintf(buf, "%.17g", value);

        return set_value_buffer(dest, header, header_mask, buf);
    }
    
    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, bool value)
    {
        return strcpy_insitu(dest, header, header_mask, value ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false"));
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, long long value)
    {
        char buf[128];
        sprintf(buf, "%lld", value);
    
        return set_value_buffer(dest, header, header_mask, buf);
    }

    PUGI__FN bool set_value_convert(char_t*& dest, uintptr_t& header, uintptr_t header_mask, unsigned long long value)
    {
        char buf[128];
        sprintf(buf, "%llu", value);
    
        return set_value_buffer(dest, header, header_mask, buf);
    }
#endif

    // we need to get length of entire file to load it in memory; the only (relatively) sane way to do it is via seek/tell trick
    PUGI__FN xml_parse_status get_file_size(FILE* file, size_t& out_result)
    {
    #if defined(PUGI__MSVC_CRT_VERSION) && PUGI__MSVC_CRT_VERSION >= 1400 && !defined(_WIN32_WCE)
        // there are 64-bit versions of fseek/ftell, let's use them
        typedef __int64 length_type;

        _fseeki64(file, 0, SEEK_END);
        length_type length = _ftelli64(file);
        _fseeki64(file, 0, SEEK_SET);
    #elif defined(__MINGW32__) && !defined(__NO_MINGW_LFS) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR))
        // there are 64-bit versions of fseek/ftell, let's use them
        typedef off64_t length_type;

        fseeko64(file, 0, SEEK_END);
        length_type length = ftello64(file);
        fseeko64(file, 0, SEEK_SET);
    #else
        // if this is a 32-bit OS, long is enough; if this is a unix system, long is 64-bit, which is enough; otherwise we can't do anything anyway.
        typedef long length_type;

        fseek(file, 0, SEEK_END);
        length_type length = ftell(file);
        fseek(file, 0, SEEK_SET);
    #endif

        // check for I/O errors
        if (length < 0) return status_io_error;
        
        // check for overflow
        size_t result = static_cast<size_t>(length);

        if (static_cast<length_type>(result) != length) return status_out_of_memory;

        // finalize
        out_result = result;

        return status_ok;
    }

    PUGI__FN size_t zero_terminate_buffer(void* buffer, size_t size, xml_encoding encoding) 
    {
        // We only need to zero-terminate if encoding conversion does not do it for us
    #ifdef PUGIXML_WCHAR_MODE
        xml_encoding wchar_encoding = get_wchar_encoding();

        if (encoding == wchar_encoding || need_endian_swap_utf(encoding, wchar_encoding))
        {
            size_t length = size / sizeof(char_t);

            static_cast<char_t*>(buffer)[length] = 0;
            return (length + 1) * sizeof(char_t);
        }
    #else
        if (encoding == encoding_utf8)
        {
            static_cast<char*>(buffer)[size] = 0;
            return size + 1;
        }
    #endif

        return size;
    }

    PUGI__FN xml_parse_result load_file_impl(xml_document& doc, FILE* file, unsigned int options, xml_encoding encoding)
    {
        if (!file) return make_parse_result(status_file_not_found);

        // get file size (can result in I/O errors)
        size_t size = 0;
        xml_parse_status size_status = get_file_size(file, size);

        if (size_status != status_ok)
        {
            fclose(file);
            return make_parse_result(size_status);
        }
        
        size_t max_suffix_size = sizeof(char_t);

        // allocate buffer for the whole file
        char* contents = static_cast<char*>(xml_memory::allocate(size + max_suffix_size));

        if (!contents)
        {
            fclose(file);
            return make_parse_result(status_out_of_memory);
        }

        // read file in memory
        size_t read_size = fread(contents, 1, size, file);
        fclose(file);

        if (read_size != size)
        {
            xml_memory::deallocate(contents);
            return make_parse_result(status_io_error);
        }

        xml_encoding real_encoding = get_buffer_encoding(encoding, contents, size);
        
        return doc.load_buffer_inplace_own(contents, zero_terminate_buffer(contents, size, real_encoding), options, real_encoding);
    }

#ifndef PUGIXML_NO_STL
    template <typename T> struct xml_stream_chunk
    {
        static xml_stream_chunk* create()
        {
            void* memory = xml_memory::allocate(sizeof(xml_stream_chunk));
            
            return new (memory) xml_stream_chunk();
        }

        static void destroy(void* ptr)
        {
            xml_stream_chunk* chunk = static_cast<xml_stream_chunk*>(ptr);

            // free chunk chain
            while (chunk)
            {
                xml_stream_chunk* next_ = chunk->next;

                xml_memory::deallocate(chunk);

                chunk = next_;
            }
        }

        xml_stream_chunk(): next(0), size(0)
        {
        }

        xml_stream_chunk* next;
        size_t size;

        T data[xml_memory_page_size / sizeof(T)];
    };

    template <typename T> PUGI__FN xml_parse_status load_stream_data_noseek(std::basic_istream<T>& stream, void** out_buffer, size_t* out_size)
    {
        buffer_holder chunks(0, xml_stream_chunk<T>::destroy);

        // read file to a chunk list
        size_t total = 0;
        xml_stream_chunk<T>* last = 0;

        while (!stream.eof())
        {
            // allocate new chunk
            xml_stream_chunk<T>* chunk = xml_stream_chunk<T>::create();
            if (!chunk) return status_out_of_memory;

            // append chunk to list
            if (last) last = last->next = chunk;
            else chunks.data = last = chunk;

            // read data to chunk
            stream.read(chunk->data, static_cast<std::streamsize>(sizeof(chunk->data) / sizeof(T)));
            chunk->size = static_cast<size_t>(stream.gcount()) * sizeof(T);

            // read may set failbit | eofbit in case gcount() is less than read length, so check for other I/O errors
            if (stream.bad() || (!stream.eof() && stream.fail())) return status_io_error;

            // guard against huge files (chunk size is small enough to make this overflow check work)
            if (total + chunk->size < total) return status_out_of_memory;
            total += chunk->size;
        }

        size_t max_suffix_size = sizeof(char_t);

        // copy chunk list to a contiguous buffer
        char* buffer = static_cast<char*>(xml_memory::allocate(total + max_suffix_size));
        if (!buffer) return status_out_of_memory;

        char* write = buffer;

        for (xml_stream_chunk<T>* chunk = static_cast<xml_stream_chunk<T>*>(chunks.data); chunk; chunk = chunk->next)
        {
            assert(write + chunk->size <= buffer + total);
            memcpy(write, chunk->data, chunk->size);
            write += chunk->size;
        }

        assert(write == buffer + total);

        // return buffer
        *out_buffer = buffer;
        *out_size = total;

        return status_ok;
    }

    template <typename T> PUGI__FN xml_parse_status load_stream_data_seek(std::basic_istream<T>& stream, void** out_buffer, size_t* out_size)
    {
        // get length of remaining data in stream
        typename std::basic_istream<T>::pos_type pos = stream.tellg();
        stream.seekg(0, std::ios::end);
        std::streamoff length = stream.tellg() - pos;
        stream.seekg(pos);

        if (stream.fail() || pos < 0) return status_io_error;

        // guard against huge files
        size_t read_length = static_cast<size_t>(length);

        if (static_cast<std::streamsize>(read_length) != length || length < 0) return status_out_of_memory;

        size_t max_suffix_size = sizeof(char_t);

        // read stream data into memory (guard against stream exceptions with buffer holder)
        buffer_holder buffer(xml_memory::allocate(read_length * sizeof(T) + max_suffix_size), xml_memory::deallocate);
        if (!buffer.data) return status_out_of_memory;

        stream.read(static_cast<T*>(buffer.data), static_cast<std::streamsize>(read_length));

        // read may set failbit | eofbit in case gcount() is less than read_length (i.e. line ending conversion), so check for other I/O errors
        if (stream.bad() || (!stream.eof() && stream.fail())) return status_io_error;

        // return buffer
        size_t actual_length = static_cast<size_t>(stream.gcount());
        assert(actual_length <= read_length);
        
        *out_buffer = buffer.release();
        *out_size = actual_length * sizeof(T);

        return status_ok;
    }

    template <typename T> PUGI__FN xml_parse_result load_stream_impl(xml_document& doc, std::basic_istream<T>& stream, unsigned int options, xml_encoding encoding)
    {
        void* buffer = 0;
        size_t size = 0;
        xml_parse_status status = status_ok;

        // if stream has an error bit set, bail out (otherwise tellg() can fail and we'll clear error bits)
        if (stream.fail()) return make_parse_result(status_io_error);

        // load stream to memory (using seek-based implementation if possible, since it's faster and takes less memory)
        if (stream.tellg() < 0)
        {
            stream.clear(); // clear error flags that could be set by a failing tellg
            status = load_stream_data_noseek(stream, &buffer, &size);
        }
        else
            status = load_stream_data_seek(stream, &buffer, &size);

        if (status != status_ok) return make_parse_result(status);

        xml_encoding real_encoding = get_buffer_encoding(encoding, buffer, size);
        
        return doc.load_buffer_inplace_own(buffer, zero_terminate_buffer(buffer, size, real_encoding), options, real_encoding);
    }
#endif

#if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__) || (defined(__MINGW32__) && (!defined(__STRICT_ANSI__) || defined(__MINGW64_VERSION_MAJOR)))
    PUGI__FN FILE* open_file_wide(const wchar_t* path, const wchar_t* mode)
    {
        return _wfopen(path, mode);
    }
#else
    PUGI__FN char* convert_path_heap(const wchar_t* str)
    {
        assert(str);

        // first pass: get length in utf8 characters
        size_t length = strlength_wide(str);
        size_t size = as_utf8_begin(str, length);

        // allocate resulting string
        char* result = static_cast<char*>(xml_memory::allocate(size + 1));
        if (!result) return 0;

        // second pass: convert to utf8
        as_utf8_end(result, size, str, length);

        return result;
    }

    PUGI__FN FILE* open_file_wide(const wchar_t* path, const wchar_t* mode)
    {
        // there is no standard function to open wide paths, so our best bet is to try utf8 path
        char* path_utf8 = convert_path_heap(path);
        if (!path_utf8) return 0;

        // convert mode to ASCII (we mirror _wfopen interface)
        char mode_ascii[4] = {0};
        for (size_t i = 0; mode[i]; ++i) mode_ascii[i] = static_cast<char>(mode[i]);

        // try to open the utf8 path
        FILE* result = fopen(path_utf8, mode_ascii);

        // free dummy buffer
        xml_memory::deallocate(path_utf8);

        return result;
    }
#endif

    PUGI__FN bool save_file_impl(const xml_document& doc, FILE* file, const char_t* indent, unsigned int flags, xml_encoding encoding)
    {
        if (!file) return false;

        xml_writer_file writer(file);
        doc.save(writer, indent, flags, encoding);

        int result = ferror(file);

        fclose(file);

        return result == 0;
    }

    PUGI__FN xml_parse_result load_buffer_impl(xml_document_struct* doc, xml_node_struct* root, void* contents, size_t size, unsigned int options, xml_encoding encoding, bool is_mutable, bool own, char_t** out_buffer)
    {
        // check input buffer
        if (!contents && size) return make_parse_result(status_io_error);

        // get actual encoding
        xml_encoding buffer_encoding = impl::get_buffer_encoding(encoding, contents, size);

        // get private buffer
        char_t* buffer = 0;
        size_t length = 0;

        if (!impl::convert_buffer(buffer, length, buffer_encoding, contents, size, is_mutable)) return impl::make_parse_result(status_out_of_memory);
        
        // delete original buffer if we performed a conversion
        if (own && buffer != contents && contents) impl::xml_memory::deallocate(contents);

        // store buffer for offset_debug
        doc->buffer = buffer;

        // parse
        xml_parse_result res = impl::xml_parser::parse(buffer, length, doc, root, options);

        // remember encoding
        res.encoding = buffer_encoding;

        // grab onto buffer if it's our buffer, user is responsible for deallocating contents himself
        if (own || buffer != contents) *out_buffer = buffer;

        return res;
    }
PUGI__NS_END

namespace pugi
{
    PUGI__FN xml_writer_file::xml_writer_file(void* file_): file(file_)
    {
    }

    PUGI__FN void xml_writer_file::write(const void* data, size_t size)
    {
        size_t result = fwrite(data, 1, size, static_cast<FILE*>(file));
        (void)!result; // unfortunately we can't do proper error handling here
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN xml_writer_stream::xml_writer_stream(std::basic_ostream<char, std::char_traits<char> >& stream): narrow_stream(&stream), wide_stream(0)
    {
    }

    PUGI__FN xml_writer_stream::xml_writer_stream(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream): narrow_stream(0), wide_stream(&stream)
    {
    }

    PUGI__FN void xml_writer_stream::write(const void* data, size_t size)
    {
        if (narrow_stream)
        {
            assert(!wide_stream);
            narrow_stream->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
        }
        else
        {
            assert(wide_stream);
            assert(size % sizeof(wchar_t) == 0);

            wide_stream->write(reinterpret_cast<const wchar_t*>(data), static_cast<std::streamsize>(size / sizeof(wchar_t)));
        }
    }
#endif

    PUGI__FN xml_tree_walker::xml_tree_walker(): _depth(0)
    {
    }
    
    PUGI__FN xml_tree_walker::~xml_tree_walker()
    {
    }

    PUGI__FN int xml_tree_walker::depth() const
    {
        return _depth;
    }

    PUGI__FN bool xml_tree_walker::begin(xml_node&)
    {
        return true;
    }

    PUGI__FN bool xml_tree_walker::end(xml_node&)
    {
        return true;
    }

    PUGI__FN xml_attribute::xml_attribute(): _attr(0)
    {
    }

    PUGI__FN xml_attribute::xml_attribute(xml_attribute_struct* attr): _attr(attr)
    {
    }

    PUGI__FN static void unspecified_bool_xml_attribute(xml_attribute***)
    {
    }

    PUGI__FN xml_attribute::operator xml_attribute::unspecified_bool_type() const
    {
        return _attr ? unspecified_bool_xml_attribute : 0;
    }

    PUGI__FN bool xml_attribute::operator!() const
    {
        return !_attr;
    }

    PUGI__FN bool xml_attribute::operator==(const xml_attribute& r) const
    {
        return (_attr == r._attr);
    }
    
    PUGI__FN bool xml_attribute::operator!=(const xml_attribute& r) const
    {
        return (_attr != r._attr);
    }

    PUGI__FN bool xml_attribute::operator<(const xml_attribute& r) const
    {
        return (_attr < r._attr);
    }
    
    PUGI__FN bool xml_attribute::operator>(const xml_attribute& r) const
    {
        return (_attr > r._attr);
    }
    
    PUGI__FN bool xml_attribute::operator<=(const xml_attribute& r) const
    {
        return (_attr <= r._attr);
    }
    
    PUGI__FN bool xml_attribute::operator>=(const xml_attribute& r) const
    {
        return (_attr >= r._attr);
    }

    PUGI__FN xml_attribute xml_attribute::next_attribute() const
    {
        return _attr ? xml_attribute(_attr->next_attribute) : xml_attribute();
    }

    PUGI__FN xml_attribute xml_attribute::previous_attribute() const
    {
        return _attr && _attr->prev_attribute_c->next_attribute ? xml_attribute(_attr->prev_attribute_c) : xml_attribute();
    }

    PUGI__FN const char_t* xml_attribute::as_string(const char_t* def) const
    {
        return (_attr && _attr->value) ? _attr->value : def;
    }

    PUGI__FN int xml_attribute::as_int(int def) const
    {
        return impl::get_value_int(_attr ? _attr->value : 0, def);
    }

    PUGI__FN unsigned int xml_attribute::as_uint(unsigned int def) const
    {
        return impl::get_value_uint(_attr ? _attr->value : 0, def);
    }

    PUGI__FN double xml_attribute::as_double(double def) const
    {
        return impl::get_value_double(_attr ? _attr->value : 0, def);
    }

    PUGI__FN float xml_attribute::as_float(float def) const
    {
        return impl::get_value_float(_attr ? _attr->value : 0, def);
    }

    PUGI__FN bool xml_attribute::as_bool(bool def) const
    {
        return impl::get_value_bool(_attr ? _attr->value : 0, def);
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN long long xml_attribute::as_llong(long long def) const
    {
        return impl::get_value_llong(_attr ? _attr->value : 0, def);
    }

    PUGI__FN unsigned long long xml_attribute::as_ullong(unsigned long long def) const
    {
        return impl::get_value_ullong(_attr ? _attr->value : 0, def);
    }
#endif

    PUGI__FN bool xml_attribute::empty() const
    {
        return !_attr;
    }

    PUGI__FN const char_t* xml_attribute::name() const
    {
        return (_attr && _attr->name) ? _attr->name : PUGIXML_TEXT("");
    }

    PUGI__FN const char_t* xml_attribute::value() const
    {
        return (_attr && _attr->value) ? _attr->value : PUGIXML_TEXT("");
    }

    PUGI__FN size_t xml_attribute::hash_value() const
    {
        return static_cast<size_t>(reinterpret_cast<uintptr_t>(_attr) / sizeof(xml_attribute_struct));
    }

    PUGI__FN xml_attribute_struct* xml_attribute::internal_object() const
    {
        return _attr;
    }

    PUGI__FN xml_attribute& xml_attribute::operator=(const char_t* rhs)
    {
        set_value(rhs);
        return *this;
    }
    
    PUGI__FN xml_attribute& xml_attribute::operator=(int rhs)
    {
        set_value(rhs);
        return *this;
    }

    PUGI__FN xml_attribute& xml_attribute::operator=(unsigned int rhs)
    {
        set_value(rhs);
        return *this;
    }

    PUGI__FN xml_attribute& xml_attribute::operator=(double rhs)
    {
        set_value(rhs);
        return *this;
    }
    
    PUGI__FN xml_attribute& xml_attribute::operator=(float rhs)
    {
        set_value(rhs);
        return *this;
    }
    
    PUGI__FN xml_attribute& xml_attribute::operator=(bool rhs)
    {
        set_value(rhs);
        return *this;
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN xml_attribute& xml_attribute::operator=(long long rhs)
    {
        set_value(rhs);
        return *this;
    }

    PUGI__FN xml_attribute& xml_attribute::operator=(unsigned long long rhs)
    {
        set_value(rhs);
        return *this;
    }
#endif

    PUGI__FN bool xml_attribute::set_name(const char_t* rhs)
    {
        if (!_attr) return false;
        
        return impl::strcpy_insitu(_attr->name, _attr->header, impl::xml_memory_page_name_allocated_mask, rhs);
    }
        
    PUGI__FN bool xml_attribute::set_value(const char_t* rhs)
    {
        if (!_attr) return false;

        return impl::strcpy_insitu(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }

    PUGI__FN bool xml_attribute::set_value(int rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }

    PUGI__FN bool xml_attribute::set_value(unsigned int rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }

    PUGI__FN bool xml_attribute::set_value(double rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }
    
    PUGI__FN bool xml_attribute::set_value(float rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }
    
    PUGI__FN bool xml_attribute::set_value(bool rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN bool xml_attribute::set_value(long long rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }

    PUGI__FN bool xml_attribute::set_value(unsigned long long rhs)
    {
        if (!_attr) return false;

        return impl::set_value_convert(_attr->value, _attr->header, impl::xml_memory_page_value_allocated_mask, rhs);
    }
#endif

#ifdef __BORLANDC__
    PUGI__FN bool operator&&(const xml_attribute& lhs, bool rhs)
    {
        return (bool)lhs && rhs;
    }

    PUGI__FN bool operator||(const xml_attribute& lhs, bool rhs)
    {
        return (bool)lhs || rhs;
    }
#endif

    PUGI__FN xml_node::xml_node(): _root(0)
    {
    }

    PUGI__FN xml_node::xml_node(xml_node_struct* p): _root(p)
    {
    }
    
    PUGI__FN static void unspecified_bool_xml_node(xml_node***)
    {
    }

    PUGI__FN xml_node::operator xml_node::unspecified_bool_type() const
    {
        return _root ? unspecified_bool_xml_node : 0;
    }

    PUGI__FN bool xml_node::operator!() const
    {
        return !_root;
    }

    PUGI__FN xml_node::iterator xml_node::begin() const
    {
        return iterator(_root ? _root->first_child : 0, _root);
    }

    PUGI__FN xml_node::iterator xml_node::end() const
    {
        return iterator(0, _root);
    }
    
    PUGI__FN xml_node::attribute_iterator xml_node::attributes_begin() const
    {
        return attribute_iterator(_root ? _root->first_attribute : 0, _root);
    }

    PUGI__FN xml_node::attribute_iterator xml_node::attributes_end() const
    {
        return attribute_iterator(0, _root);
    }
    
    PUGI__FN xml_object_range<xml_node_iterator> xml_node::children() const
    {
        return xml_object_range<xml_node_iterator>(begin(), end());
    }

    PUGI__FN xml_object_range<xml_named_node_iterator> xml_node::children(const char_t* name_) const
    {
        return xml_object_range<xml_named_node_iterator>(xml_named_node_iterator(child(name_)._root, _root, name_), xml_named_node_iterator(0, _root, name_));
    }

    PUGI__FN xml_object_range<xml_attribute_iterator> xml_node::attributes() const
    {
        return xml_object_range<xml_attribute_iterator>(attributes_begin(), attributes_end());
    }

    PUGI__FN bool xml_node::operator==(const xml_node& r) const
    {
        return (_root == r._root);
    }

    PUGI__FN bool xml_node::operator!=(const xml_node& r) const
    {
        return (_root != r._root);
    }

    PUGI__FN bool xml_node::operator<(const xml_node& r) const
    {
        return (_root < r._root);
    }
    
    PUGI__FN bool xml_node::operator>(const xml_node& r) const
    {
        return (_root > r._root);
    }
    
    PUGI__FN bool xml_node::operator<=(const xml_node& r) const
    {
        return (_root <= r._root);
    }
    
    PUGI__FN bool xml_node::operator>=(const xml_node& r) const
    {
        return (_root >= r._root);
    }

    PUGI__FN bool xml_node::empty() const
    {
        return !_root;
    }
    
    PUGI__FN const char_t* xml_node::name() const
    {
        return (_root && _root->name) ? _root->name : PUGIXML_TEXT("");
    }

    PUGI__FN xml_node_type xml_node::type() const
    {
        return _root ? PUGI__NODETYPE(_root) : node_null;
    }
    
    PUGI__FN const char_t* xml_node::value() const
    {
        return (_root && _root->value) ? _root->value : PUGIXML_TEXT("");
    }
    
    PUGI__FN xml_node xml_node::child(const char_t* name_) const
    {
        if (!_root) return xml_node();

        for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
            if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

        return xml_node();
    }

    PUGI__FN xml_attribute xml_node::attribute(const char_t* name_) const
    {
        if (!_root) return xml_attribute();

        for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
            if (i->name && impl::strequal(name_, i->name))
                return xml_attribute(i);
        
        return xml_attribute();
    }
    
    PUGI__FN xml_node xml_node::next_sibling(const char_t* name_) const
    {
        if (!_root) return xml_node();
        
        for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
            if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

        return xml_node();
    }

    PUGI__FN xml_node xml_node::next_sibling() const
    {
        return _root ? xml_node(_root->next_sibling) : xml_node();
    }

    PUGI__FN xml_node xml_node::previous_sibling(const char_t* name_) const
    {
        if (!_root) return xml_node();
        
        for (xml_node_struct* i = _root->prev_sibling_c; i->next_sibling; i = i->prev_sibling_c)
            if (i->name && impl::strequal(name_, i->name)) return xml_node(i);

        return xml_node();
    }

    PUGI__FN xml_node xml_node::previous_sibling() const
    {
        if (!_root) return xml_node();
        
        if (_root->prev_sibling_c->next_sibling) return xml_node(_root->prev_sibling_c);
        else return xml_node();
    }

    PUGI__FN xml_node xml_node::parent() const
    {
        return _root ? xml_node(_root->parent) : xml_node();
    }

    PUGI__FN xml_node xml_node::root() const
    {
        return _root ? xml_node(&impl::get_document(_root)) : xml_node();
    }

    PUGI__FN xml_text xml_node::text() const
    {
        return xml_text(_root);
    }

    PUGI__FN const char_t* xml_node::child_value() const
    {
        if (!_root) return PUGIXML_TEXT("");
        
        for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
            if (i->value && impl::is_text_node(i))
                return i->value;

        return PUGIXML_TEXT("");
    }

    PUGI__FN const char_t* xml_node::child_value(const char_t* name_) const
    {
        return child(name_).child_value();
    }

    PUGI__FN xml_attribute xml_node::first_attribute() const
    {
        return _root ? xml_attribute(_root->first_attribute) : xml_attribute();
    }

    PUGI__FN xml_attribute xml_node::last_attribute() const
    {
        return _root && _root->first_attribute ? xml_attribute(_root->first_attribute->prev_attribute_c) : xml_attribute();
    }

    PUGI__FN xml_node xml_node::first_child() const
    {
        return _root ? xml_node(_root->first_child) : xml_node();
    }

    PUGI__FN xml_node xml_node::last_child() const
    {
        return _root && _root->first_child ? xml_node(_root->first_child->prev_sibling_c) : xml_node();
    }

    PUGI__FN bool xml_node::set_name(const char_t* rhs)
    {
        switch (type())
        {
        case node_pi:
        case node_declaration:
        case node_element:
            return impl::strcpy_insitu(_root->name, _root->header, impl::xml_memory_page_name_allocated_mask, rhs);

        default:
            return false;
        }
    }
        
    PUGI__FN bool xml_node::set_value(const char_t* rhs)
    {
        switch (type())
        {
        case node_pi:
        case node_cdata:
        case node_pcdata:
        case node_comment:
        case node_doctype:
            return impl::strcpy_insitu(_root->value, _root->header, impl::xml_memory_page_value_allocated_mask, rhs);

        default:
            return false;
        }
    }

    PUGI__FN xml_attribute xml_node::append_attribute(const char_t* name_)
    {
        if (!impl::allow_insert_attribute(type())) return xml_attribute();
        
        xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
        if (!a) return xml_attribute();

        impl::append_attribute(a._attr, _root);

        a.set_name(name_);
        
        return a;
    }

    PUGI__FN xml_attribute xml_node::prepend_attribute(const char_t* name_)
    {
        if (!impl::allow_insert_attribute(type())) return xml_attribute();
        
        xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
        if (!a) return xml_attribute();

        impl::prepend_attribute(a._attr, _root);

        a.set_name(name_);

        return a;
    }

    PUGI__FN xml_attribute xml_node::insert_attribute_after(const char_t* name_, const xml_attribute& attr)
    {
        if (!impl::allow_insert_attribute(type())) return xml_attribute();
        if (!attr || !impl::is_attribute_of(attr._attr, _root)) return xml_attribute();
        
        xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
        if (!a) return xml_attribute();

        impl::insert_attribute_after(a._attr, attr._attr, _root);

        a.set_name(name_);

        return a;
    }

    PUGI__FN xml_attribute xml_node::insert_attribute_before(const char_t* name_, const xml_attribute& attr)
    {
        if (!impl::allow_insert_attribute(type())) return xml_attribute();
        if (!attr || !impl::is_attribute_of(attr._attr, _root)) return xml_attribute();
        
        xml_attribute a(impl::allocate_attribute(impl::get_allocator(_root)));
        if (!a) return xml_attribute();

        impl::insert_attribute_before(a._attr, attr._attr, _root);

        a.set_name(name_);

        return a;
    }

    PUGI__FN xml_attribute xml_node::append_copy(const xml_attribute& proto)
    {
        if (!proto) return xml_attribute();

        xml_attribute result = append_attribute(proto.name());
        result.set_value(proto.value());

        return result;
    }

    PUGI__FN xml_attribute xml_node::prepend_copy(const xml_attribute& proto)
    {
        if (!proto) return xml_attribute();

        xml_attribute result = prepend_attribute(proto.name());
        result.set_value(proto.value());

        return result;
    }

    PUGI__FN xml_attribute xml_node::insert_copy_after(const xml_attribute& proto, const xml_attribute& attr)
    {
        if (!proto) return xml_attribute();

        xml_attribute result = insert_attribute_after(proto.name(), attr);
        result.set_value(proto.value());

        return result;
    }

    PUGI__FN xml_attribute xml_node::insert_copy_before(const xml_attribute& proto, const xml_attribute& attr)
    {
        if (!proto) return xml_attribute();

        xml_attribute result = insert_attribute_before(proto.name(), attr);
        result.set_value(proto.value());

        return result;
    }

    PUGI__FN xml_node xml_node::append_child(xml_node_type type_)
    {
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        
        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::append_node(n._root, _root);

        if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

        return n;
    }

    PUGI__FN xml_node xml_node::prepend_child(xml_node_type type_)
    {
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        
        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::prepend_node(n._root, _root);
                
        if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

        return n;
    }

    PUGI__FN xml_node xml_node::insert_child_before(xml_node_type type_, const xml_node& node)
    {
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();
    
        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::insert_node_before(n._root, node._root);

        if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

        return n;
    }

    PUGI__FN xml_node xml_node::insert_child_after(xml_node_type type_, const xml_node& node)
    {
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();
    
        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::insert_node_after(n._root, node._root);

        if (type_ == node_declaration) n.set_name(PUGIXML_TEXT("xml"));

        return n;
    }

    PUGI__FN xml_node xml_node::append_child(const char_t* name_)
    {
        xml_node result = append_child(node_element);

        result.set_name(name_);

        return result;
    }

    PUGI__FN xml_node xml_node::prepend_child(const char_t* name_)
    {
        xml_node result = prepend_child(node_element);

        result.set_name(name_);

        return result;
    }

    PUGI__FN xml_node xml_node::insert_child_after(const char_t* name_, const xml_node& node)
    {
        xml_node result = insert_child_after(node_element, node);

        result.set_name(name_);

        return result;
    }

    PUGI__FN xml_node xml_node::insert_child_before(const char_t* name_, const xml_node& node)
    {
        xml_node result = insert_child_before(node_element, node);

        result.set_name(name_);

        return result;
    }

    PUGI__FN xml_node xml_node::append_copy(const xml_node& proto)
    {
        xml_node_type type_ = proto.type();
        if (!impl::allow_insert_child(type(), type_)) return xml_node();

        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::append_node(n._root, _root);
        impl::node_copy_tree(n._root, proto._root);

        return n;
    }

    PUGI__FN xml_node xml_node::prepend_copy(const xml_node& proto)
    {
        xml_node_type type_ = proto.type();
        if (!impl::allow_insert_child(type(), type_)) return xml_node();

        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::prepend_node(n._root, _root);
        impl::node_copy_tree(n._root, proto._root);

        return n;
    }

    PUGI__FN xml_node xml_node::insert_copy_after(const xml_node& proto, const xml_node& node)
    {
        xml_node_type type_ = proto.type();
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();

        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::insert_node_after(n._root, node._root);
        impl::node_copy_tree(n._root, proto._root);

        return n;
    }

    PUGI__FN xml_node xml_node::insert_copy_before(const xml_node& proto, const xml_node& node)
    {
        xml_node_type type_ = proto.type();
        if (!impl::allow_insert_child(type(), type_)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();

        xml_node n(impl::allocate_node(impl::get_allocator(_root), type_));
        if (!n) return xml_node();

        impl::insert_node_before(n._root, node._root);
        impl::node_copy_tree(n._root, proto._root);

        return n;
    }

    PUGI__FN xml_node xml_node::append_move(const xml_node& moved)
    {
        if (!impl::allow_move(*this, moved)) return xml_node();

        // disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
        impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

        impl::remove_node(moved._root);
        impl::append_node(moved._root, _root);

        return moved;
    }

    PUGI__FN xml_node xml_node::prepend_move(const xml_node& moved)
    {
        if (!impl::allow_move(*this, moved)) return xml_node();

        // disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
        impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

        impl::remove_node(moved._root);
        impl::prepend_node(moved._root, _root);

        return moved;
    }

    PUGI__FN xml_node xml_node::insert_move_after(const xml_node& moved, const xml_node& node)
    {
        if (!impl::allow_move(*this, moved)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();
        if (moved._root == node._root) return xml_node();

        // disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
        impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

        impl::remove_node(moved._root);
        impl::insert_node_after(moved._root, node._root);

        return moved;
    }

    PUGI__FN xml_node xml_node::insert_move_before(const xml_node& moved, const xml_node& node)
    {
        if (!impl::allow_move(*this, moved)) return xml_node();
        if (!node._root || node._root->parent != _root) return xml_node();
        if (moved._root == node._root) return xml_node();

        // disable document_buffer_order optimization since moving nodes around changes document order without changing buffer pointers
        impl::get_document(_root).header |= impl::xml_memory_page_contents_shared_mask;

        impl::remove_node(moved._root);
        impl::insert_node_before(moved._root, node._root);

        return moved;
    }

    PUGI__FN bool xml_node::remove_attribute(const char_t* name_)
    {
        return remove_attribute(attribute(name_));
    }

    PUGI__FN bool xml_node::remove_attribute(const xml_attribute& a)
    {
        if (!_root || !a._attr) return false;
        if (!impl::is_attribute_of(a._attr, _root)) return false;

        impl::remove_attribute(a._attr, _root);
        impl::destroy_attribute(a._attr, impl::get_allocator(_root));

        return true;
    }

    PUGI__FN bool xml_node::remove_child(const char_t* name_)
    {
        return remove_child(child(name_));
    }

    PUGI__FN bool xml_node::remove_child(const xml_node& n)
    {
        if (!_root || !n._root || n._root->parent != _root) return false;

        impl::remove_node(n._root);
        impl::destroy_node(n._root, impl::get_allocator(_root));

        return true;
    }

    PUGI__FN xml_parse_result xml_node::append_buffer(const void* contents, size_t size, unsigned int options, xml_encoding encoding)
    {
        // append_buffer is only valid for elements/documents
        if (!impl::allow_insert_child(type(), node_element)) return impl::make_parse_result(status_append_invalid_root);

        // get document node
        impl::xml_document_struct* doc = &impl::get_document(_root);

        // disable document_buffer_order optimization since in a document with multiple buffers comparing buffer pointers does not make sense
        doc->header |= impl::xml_memory_page_contents_shared_mask;
        
        // get extra buffer element (we'll store the document fragment buffer there so that we can deallocate it later)
        impl::xml_memory_page* page = 0;
        impl::xml_extra_buffer* extra = static_cast<impl::xml_extra_buffer*>(doc->allocate_memory(sizeof(impl::xml_extra_buffer), page));
        (void)page;

        if (!extra) return impl::make_parse_result(status_out_of_memory);

        // save name; name of the root has to be NULL before parsing - otherwise closing node mismatches will not be detected at the top level
        char_t* rootname = _root->name;
        _root->name = 0;

        // parse
        char_t* buffer = 0;
        xml_parse_result res = impl::load_buffer_impl(doc, _root, const_cast<void*>(contents), size, options, encoding, false, false, &buffer);

        // restore name
        _root->name = rootname;

        // add extra buffer to the list
        extra->buffer = buffer;
        extra->next = doc->extra_buffers;
        doc->extra_buffers = extra;

        return res;
    }

    PUGI__FN xml_node xml_node::find_child_by_attribute(const char_t* name_, const char_t* attr_name, const char_t* attr_value) const
    {
        if (!_root) return xml_node();
        
        for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
            if (i->name && impl::strequal(name_, i->name))
            {
                for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
                    if (a->name && impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value ? a->value : PUGIXML_TEXT("")))
                        return xml_node(i);
            }

        return xml_node();
    }

    PUGI__FN xml_node xml_node::find_child_by_attribute(const char_t* attr_name, const char_t* attr_value) const
    {
        if (!_root) return xml_node();
        
        for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
            for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
                if (a->name && impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value ? a->value : PUGIXML_TEXT("")))
                    return xml_node(i);

        return xml_node();
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN string_t xml_node::path(char_t delimiter) const
    {
        xml_node cursor = *this; // Make a copy.
        
        string_t result = cursor.name();

        while (cursor.parent())
        {
            cursor = cursor.parent();
            
            string_t temp = cursor.name();
            temp += delimiter;
            temp += result;
            result.swap(temp);
        }

        return result;
    }
#endif

    PUGI__FN xml_node xml_node::first_element_by_path(const char_t* path_, char_t delimiter) const
    {
        xml_node found = *this; // Current search context.

        if (!_root || !path_ || !path_[0]) return found;

        if (path_[0] == delimiter)
        {
            // Absolute path; e.g. '/foo/bar'
            found = found.root();
            ++path_;
        }

        const char_t* path_segment = path_;

        while (*path_segment == delimiter) ++path_segment;

        const char_t* path_segment_end = path_segment;

        while (*path_segment_end && *path_segment_end != delimiter) ++path_segment_end;

        if (path_segment == path_segment_end) return found;

        const char_t* next_segment = path_segment_end;

        while (*next_segment == delimiter) ++next_segment;

        if (*path_segment == '.' && path_segment + 1 == path_segment_end)
            return found.first_element_by_path(next_segment, delimiter);
        else if (*path_segment == '.' && *(path_segment+1) == '.' && path_segment + 2 == path_segment_end)
            return found.parent().first_element_by_path(next_segment, delimiter);
        else
        {
            for (xml_node_struct* j = found._root->first_child; j; j = j->next_sibling)
            {
                if (j->name && impl::strequalrange(j->name, path_segment, static_cast<size_t>(path_segment_end - path_segment)))
                {
                    xml_node subsearch = xml_node(j).first_element_by_path(next_segment, delimiter);

                    if (subsearch) return subsearch;
                }
            }

            return xml_node();
        }
    }

    PUGI__FN bool xml_node::traverse(xml_tree_walker& walker)
    {
        walker._depth = -1;
        
        xml_node arg_begin = *this;
        if (!walker.begin(arg_begin)) return false;

        xml_node cur = first_child();
                
        if (cur)
        {
            ++walker._depth;

            do 
            {
                xml_node arg_for_each = cur;
                if (!walker.for_each(arg_for_each))
                    return false;
                        
                if (cur.first_child())
                {
                    ++walker._depth;
                    cur = cur.first_child();
                }
                else if (cur.next_sibling())
                    cur = cur.next_sibling();
                else
                {
                    // Borland C++ workaround
                    while (!cur.next_sibling() && cur != *this && !cur.parent().empty())
                    {
                        --walker._depth;
                        cur = cur.parent();
                    }
                        
                    if (cur != *this)
                        cur = cur.next_sibling();
                }
            }
            while (cur && cur != *this);
        }

        assert(walker._depth == -1);

        xml_node arg_end = *this;
        return walker.end(arg_end);
    }

    PUGI__FN size_t xml_node::hash_value() const
    {
        return static_cast<size_t>(reinterpret_cast<uintptr_t>(_root) / sizeof(xml_node_struct));
    }

    PUGI__FN xml_node_struct* xml_node::internal_object() const
    {
        return _root;
    }

    PUGI__FN void xml_node::print(xml_writer& writer, const char_t* indent, unsigned int flags, xml_encoding encoding, unsigned int depth) const
    {
        if (!_root) return;

        impl::xml_buffered_writer buffered_writer(writer, encoding);

        impl::node_output(buffered_writer, _root, indent, flags, depth);
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN void xml_node::print(std::basic_ostream<char, std::char_traits<char> >& stream, const char_t* indent, unsigned int flags, xml_encoding encoding, unsigned int depth) const
    {
        xml_writer_stream writer(stream);

        print(writer, indent, flags, encoding, depth);
    }

    PUGI__FN void xml_node::print(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream, const char_t* indent, unsigned int flags, unsigned int depth) const
    {
        xml_writer_stream writer(stream);

        print(writer, indent, flags, encoding_wchar, depth);
    }
#endif

    PUGI__FN ptrdiff_t xml_node::offset_debug() const
    {
        if (!_root) return -1;

        impl::xml_document_struct& doc = impl::get_document(_root);

        // we can determine the offset reliably only if there is exactly once parse buffer
        if (!doc.buffer || doc.extra_buffers) return -1;

        switch (type())
        {
        case node_document:
            return 0;

        case node_element:
        case node_declaration:
        case node_pi:
            return _root->name && (_root->header & impl::xml_memory_page_name_allocated_or_shared_mask) == 0 ? _root->name - doc.buffer : -1;

        case node_pcdata:
        case node_cdata:
        case node_comment:
        case node_doctype:
            return _root->value && (_root->header & impl::xml_memory_page_value_allocated_or_shared_mask) == 0 ? _root->value - doc.buffer : -1;

        default:
            return -1;
        }
    }

#ifdef __BORLANDC__
    PUGI__FN bool operator&&(const xml_node& lhs, bool rhs)
    {
        return (bool)lhs && rhs;
    }

    PUGI__FN bool operator||(const xml_node& lhs, bool rhs)
    {
        return (bool)lhs || rhs;
    }
#endif

    PUGI__FN xml_text::xml_text(xml_node_struct* root): _root(root)
    {
    }

    PUGI__FN xml_node_struct* xml_text::_data() const
    {
        if (!_root || impl::is_text_node(_root)) return _root;

        for (xml_node_struct* node = _root->first_child; node; node = node->next_sibling)
            if (impl::is_text_node(node))
                return node;

        return 0;
    }

    PUGI__FN xml_node_struct* xml_text::_data_new()
    {
        xml_node_struct* d = _data();
        if (d) return d;

        return xml_node(_root).append_child(node_pcdata).internal_object();
    }

    PUGI__FN xml_text::xml_text(): _root(0)
    {
    }

    PUGI__FN static void unspecified_bool_xml_text(xml_text***)
    {
    }

    PUGI__FN xml_text::operator xml_text::unspecified_bool_type() const
    {
        return _data() ? unspecified_bool_xml_text : 0;
    }

    PUGI__FN bool xml_text::operator!() const
    {
        return !_data();
    }

    PUGI__FN bool xml_text::empty() const
    {
        return _data() == 0;
    }

    PUGI__FN const char_t* xml_text::get() const
    {
        xml_node_struct* d = _data();

        return (d && d->value) ? d->value : PUGIXML_TEXT("");
    }

    PUGI__FN const char_t* xml_text::as_string(const char_t* def) const
    {
        xml_node_struct* d = _data();

        return (d && d->value) ? d->value : def;
    }

    PUGI__FN int xml_text::as_int(int def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_int(d ? d->value : 0, def);
    }

    PUGI__FN unsigned int xml_text::as_uint(unsigned int def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_uint(d ? d->value : 0, def);
    }

    PUGI__FN double xml_text::as_double(double def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_double(d ? d->value : 0, def);
    }

    PUGI__FN float xml_text::as_float(float def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_float(d ? d->value : 0, def);
    }

    PUGI__FN bool xml_text::as_bool(bool def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_bool(d ? d->value : 0, def);
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN long long xml_text::as_llong(long long def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_llong(d ? d->value : 0, def);
    }

    PUGI__FN unsigned long long xml_text::as_ullong(unsigned long long def) const
    {
        xml_node_struct* d = _data();

        return impl::get_value_ullong(d ? d->value : 0, def);
    }
#endif

    PUGI__FN bool xml_text::set(const char_t* rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::strcpy_insitu(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(int rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(unsigned int rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(float rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(double rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(bool rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN bool xml_text::set(long long rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }

    PUGI__FN bool xml_text::set(unsigned long long rhs)
    {
        xml_node_struct* dn = _data_new();

        return dn ? impl::set_value_convert(dn->value, dn->header, impl::xml_memory_page_value_allocated_mask, rhs) : false;
    }
#endif

    PUGI__FN xml_text& xml_text::operator=(const char_t* rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(int rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(unsigned int rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(double rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(float rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(bool rhs)
    {
        set(rhs);
        return *this;
    }

#ifdef PUGIXML_HAS_LONG_LONG
    PUGI__FN xml_text& xml_text::operator=(long long rhs)
    {
        set(rhs);
        return *this;
    }

    PUGI__FN xml_text& xml_text::operator=(unsigned long long rhs)
    {
        set(rhs);
        return *this;
    }
#endif

    PUGI__FN xml_node xml_text::data() const
    {
        return xml_node(_data());
    }

#ifdef __BORLANDC__
    PUGI__FN bool operator&&(const xml_text& lhs, bool rhs)
    {
        return (bool)lhs && rhs;
    }

    PUGI__FN bool operator||(const xml_text& lhs, bool rhs)
    {
        return (bool)lhs || rhs;
    }
#endif

    PUGI__FN xml_node_iterator::xml_node_iterator()
    {
    }

    PUGI__FN xml_node_iterator::xml_node_iterator(const xml_node& node): _wrap(node), _parent(node.parent())
    {
    }

    PUGI__FN xml_node_iterator::xml_node_iterator(xml_node_struct* ref, xml_node_struct* parent): _wrap(ref), _parent(parent)
    {
    }

    PUGI__FN bool xml_node_iterator::operator==(const xml_node_iterator& rhs) const
    {
        return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
    }
    
    PUGI__FN bool xml_node_iterator::operator!=(const xml_node_iterator& rhs) const
    {
        return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
    }

    PUGI__FN xml_node& xml_node_iterator::operator*() const
    {
        assert(_wrap._root);
        return _wrap;
    }

    PUGI__FN xml_node* xml_node_iterator::operator->() const
    {
        assert(_wrap._root);
        return const_cast<xml_node*>(&_wrap); // BCC32 workaround
    }

    PUGI__FN const xml_node_iterator& xml_node_iterator::operator++()
    {
        assert(_wrap._root);
        _wrap._root = _wrap._root->next_sibling;
        return *this;
    }

    PUGI__FN xml_node_iterator xml_node_iterator::operator++(int)
    {
        xml_node_iterator temp = *this;
        ++*this;
        return temp;
    }

    PUGI__FN const xml_node_iterator& xml_node_iterator::operator--()
    {
        _wrap = _wrap._root ? _wrap.previous_sibling() : _parent.last_child();
        return *this;
    }

    PUGI__FN xml_node_iterator xml_node_iterator::operator--(int)
    {
        xml_node_iterator temp = *this;
        --*this;
        return temp;
    }

    PUGI__FN xml_attribute_iterator::xml_attribute_iterator()
    {
    }

    PUGI__FN xml_attribute_iterator::xml_attribute_iterator(const xml_attribute& attr, const xml_node& parent): _wrap(attr), _parent(parent)
    {
    }

    PUGI__FN xml_attribute_iterator::xml_attribute_iterator(xml_attribute_struct* ref, xml_node_struct* parent): _wrap(ref), _parent(parent)
    {
    }

    PUGI__FN bool xml_attribute_iterator::operator==(const xml_attribute_iterator& rhs) const
    {
        return _wrap._attr == rhs._wrap._attr && _parent._root == rhs._parent._root;
    }
    
    PUGI__FN bool xml_attribute_iterator::operator!=(const xml_attribute_iterator& rhs) const
    {
        return _wrap._attr != rhs._wrap._attr || _parent._root != rhs._parent._root;
    }

    PUGI__FN xml_attribute& xml_attribute_iterator::operator*() const
    {
        assert(_wrap._attr);
        return _wrap;
    }

    PUGI__FN xml_attribute* xml_attribute_iterator::operator->() const
    {
        assert(_wrap._attr);
        return const_cast<xml_attribute*>(&_wrap); // BCC32 workaround
    }

    PUGI__FN const xml_attribute_iterator& xml_attribute_iterator::operator++()
    {
        assert(_wrap._attr);
        _wrap._attr = _wrap._attr->next_attribute;
        return *this;
    }

    PUGI__FN xml_attribute_iterator xml_attribute_iterator::operator++(int)
    {
        xml_attribute_iterator temp = *this;
        ++*this;
        return temp;
    }

    PUGI__FN const xml_attribute_iterator& xml_attribute_iterator::operator--()
    {
        _wrap = _wrap._attr ? _wrap.previous_attribute() : _parent.last_attribute();
        return *this;
    }

    PUGI__FN xml_attribute_iterator xml_attribute_iterator::operator--(int)
    {
        xml_attribute_iterator temp = *this;
        --*this;
        return temp;
    }

    PUGI__FN xml_named_node_iterator::xml_named_node_iterator(): _name(0)
    {
    }

    PUGI__FN xml_named_node_iterator::xml_named_node_iterator(const xml_node& node, const char_t* name): _wrap(node), _parent(node.parent()), _name(name)
    {
    }

    PUGI__FN xml_named_node_iterator::xml_named_node_iterator(xml_node_struct* ref, xml_node_struct* parent, const char_t* name): _wrap(ref), _parent(parent), _name(name)
    {
    }

    PUGI__FN bool xml_named_node_iterator::operator==(const xml_named_node_iterator& rhs) const
    {
        return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
    }

    PUGI__FN bool xml_named_node_iterator::operator!=(const xml_named_node_iterator& rhs) const
    {
        return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
    }

    PUGI__FN xml_node& xml_named_node_iterator::operator*() const
    {
        assert(_wrap._root);
        return _wrap;
    }

    PUGI__FN xml_node* xml_named_node_iterator::operator->() const
    {
        assert(_wrap._root);
        return const_cast<xml_node*>(&_wrap); // BCC32 workaround
    }

    PUGI__FN const xml_named_node_iterator& xml_named_node_iterator::operator++()
    {
        assert(_wrap._root);
        _wrap = _wrap.next_sibling(_name);
        return *this;
    }

    PUGI__FN xml_named_node_iterator xml_named_node_iterator::operator++(int)
    {
        xml_named_node_iterator temp = *this;
        ++*this;
        return temp;
    }

    PUGI__FN const xml_named_node_iterator& xml_named_node_iterator::operator--()
    {
        if (_wrap._root)
            _wrap = _wrap.previous_sibling(_name);
        else
        {
            _wrap = _parent.last_child();

            if (!impl::strequal(_wrap.name(), _name))
                _wrap = _wrap.previous_sibling(_name);
        }

        return *this;
    }

    PUGI__FN xml_named_node_iterator xml_named_node_iterator::operator--(int)
    {
        xml_named_node_iterator temp = *this;
        --*this;
        return temp;
    }

    PUGI__FN xml_parse_result::xml_parse_result(): status(status_internal_error), offset(0), encoding(encoding_auto)
    {
    }

    PUGI__FN xml_parse_result::operator bool() const
    {
        return status == status_ok;
    }

    PUGI__FN const char* xml_parse_result::description() const
    {
        switch (status)
        {
        case status_ok: return "No error";

        case status_file_not_found: return "File was not found";
        case status_io_error: return "Error reading from file/stream";
        case status_out_of_memory: return "Could not allocate memory";
        case status_internal_error: return "Internal error occurred";

        case status_unrecognized_tag: return "Could not determine tag type";

        case status_bad_pi: return "Error parsing document declaration/processing instruction";
        case status_bad_comment: return "Error parsing comment";
        case status_bad_cdata: return "Error parsing CDATA section";
        case status_bad_doctype: return "Error parsing document type declaration";
        case status_bad_pcdata: return "Error parsing PCDATA section";
        case status_bad_start_element: return "Error parsing start element tag";
        case status_bad_attribute: return "Error parsing element attribute";
        case status_bad_end_element: return "Error parsing end element tag";
        case status_end_element_mismatch: return "Start-end tags mismatch";

        case status_append_invalid_root: return "Unable to append nodes: root is not an element or document";

        case status_no_document_element: return "No document element found";

        default: return "Unknown error";
        }
    }

    PUGI__FN xml_document::xml_document(): _buffer(0)
    {
        create();
    }

    PUGI__FN xml_document::~xml_document()
    {
        destroy();
    }

    PUGI__FN void xml_document::reset()
    {
        destroy();
        create();
    }

    PUGI__FN void xml_document::reset(const xml_document& proto)
    {
        reset();

        for (xml_node cur = proto.first_child(); cur; cur = cur.next_sibling())
            append_copy(cur);
    }

    PUGI__FN void xml_document::create()
    {
        assert(!_root);

        // initialize sentinel page
        PUGI__STATIC_ASSERT(sizeof(impl::xml_memory_page) + sizeof(impl::xml_document_struct) + impl::xml_memory_page_alignment - sizeof(void*) <= sizeof(_memory));

        // align upwards to page boundary
        void* page_memory = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(_memory) + (impl::xml_memory_page_alignment - 1)) & ~(impl::xml_memory_page_alignment - 1));

        // prepare page structure
        impl::xml_memory_page* page = impl::xml_memory_page::construct(page_memory);
        assert(page);

        page->busy_size = impl::xml_memory_page_size;

        // allocate new root
        _root = new (reinterpret_cast<char*>(page) + sizeof(impl::xml_memory_page)) impl::xml_document_struct(page);
        _root->prev_sibling_c = _root;

        // setup sentinel page
        page->allocator = static_cast<impl::xml_document_struct*>(_root);

        // verify the document allocation
        assert(reinterpret_cast<char*>(_root) + sizeof(impl::xml_document_struct) <= _memory + sizeof(_memory));
    }

    PUGI__FN void xml_document::destroy()
    {
        assert(_root);

        // destroy static storage
        if (_buffer)
        {
            impl::xml_memory::deallocate(_buffer);
            _buffer = 0;
        }

        // destroy extra buffers (note: no need to destroy linked list nodes, they're allocated using document allocator)
        for (impl::xml_extra_buffer* extra = static_cast<impl::xml_document_struct*>(_root)->extra_buffers; extra; extra = extra->next)
        {
            if (extra->buffer) impl::xml_memory::deallocate(extra->buffer);
        }

        // destroy dynamic storage, leave sentinel page (it's in static memory)
        impl::xml_memory_page* root_page = reinterpret_cast<impl::xml_memory_page*>(_root->header & impl::xml_memory_page_pointer_mask);
        assert(root_page && !root_page->prev);
        assert(reinterpret_cast<char*>(root_page) >= _memory && reinterpret_cast<char*>(root_page) < _memory + sizeof(_memory));

        for (impl::xml_memory_page* page = root_page->next; page; )
        {
            impl::xml_memory_page* next = page->next;

            impl::xml_allocator::deallocate_page(page);

            page = next;
        }

        _root = 0;
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN xml_parse_result xml_document::load(std::basic_istream<char, std::char_traits<char> >& stream, unsigned int options, xml_encoding encoding)
    {
        reset();

        return impl::load_stream_impl(*this, stream, options, encoding);
    }

    PUGI__FN xml_parse_result xml_document::load(std::basic_istream<wchar_t, std::char_traits<wchar_t> >& stream, unsigned int options)
    {
        reset();

        return impl::load_stream_impl(*this, stream, options, encoding_wchar);
    }
#endif

    PUGI__FN xml_parse_result xml_document::load_string(const char_t* contents, unsigned int options)
    {
        // Force native encoding (skip autodetection)
    #ifdef PUGIXML_WCHAR_MODE
        xml_encoding encoding = encoding_wchar;
    #else
        xml_encoding encoding = encoding_utf8;
    #endif

        return load_buffer(contents, impl::strlength(contents) * sizeof(char_t), options, encoding);
    }

    PUGI__FN xml_parse_result xml_document::load(const char_t* contents, unsigned int options)
    {
        return load_string(contents, options);
    }

    PUGI__FN xml_parse_result xml_document::load_file(const char* path_, unsigned int options, xml_encoding encoding)
    {
        reset();

        FILE* file = fopen(path_, "rb");

        return impl::load_file_impl(*this, file, options, encoding);
    }

    PUGI__FN xml_parse_result xml_document::load_file(const wchar_t* path_, unsigned int options, xml_encoding encoding)
    {
        reset();

        FILE* file = impl::open_file_wide(path_, L"rb");

        return impl::load_file_impl(*this, file, options, encoding);
    }

    PUGI__FN xml_parse_result xml_document::load_buffer(const void* contents, size_t size, unsigned int options, xml_encoding encoding)
    {
        reset();

        return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, const_cast<void*>(contents), size, options, encoding, false, false, &_buffer);
    }

    PUGI__FN xml_parse_result xml_document::load_buffer_inplace(void* contents, size_t size, unsigned int options, xml_encoding encoding)
    {
        reset();

        return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, contents, size, options, encoding, true, false, &_buffer);
    }

    PUGI__FN xml_parse_result xml_document::load_buffer_inplace_own(void* contents, size_t size, unsigned int options, xml_encoding encoding)
    {
        reset();

        return impl::load_buffer_impl(static_cast<impl::xml_document_struct*>(_root), _root, contents, size, options, encoding, true, true, &_buffer);
    }

    PUGI__FN void xml_document::save(xml_writer& writer, const char_t* indent, unsigned int flags, xml_encoding encoding) const
    {
        impl::xml_buffered_writer buffered_writer(writer, encoding);

        if ((flags & format_write_bom) && encoding != encoding_latin1)
        {
            // BOM always represents the codepoint U+FEFF, so just write it in native encoding
        #ifdef PUGIXML_WCHAR_MODE
            unsigned int bom = 0xfeff;
            buffered_writer.write(static_cast<wchar_t>(bom));
        #else
            buffered_writer.write('\xef', '\xbb', '\xbf');
        #endif
        }

        if (!(flags & format_no_declaration) && !impl::has_declaration(_root))
        {
            buffered_writer.write_string(PUGIXML_TEXT("<?xml version=\"1.0\""));
            if (encoding == encoding_latin1) buffered_writer.write_string(PUGIXML_TEXT(" encoding=\"ISO-8859-1\""));
            buffered_writer.write('?', '>');
            if (!(flags & format_raw)) buffered_writer.write('\n');
        }

        impl::node_output(buffered_writer, _root, indent, flags, 0);
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN void xml_document::save(std::basic_ostream<char, std::char_traits<char> >& stream, const char_t* indent, unsigned int flags, xml_encoding encoding) const
    {
        xml_writer_stream writer(stream);

        save(writer, indent, flags, encoding);
    }

    PUGI__FN void xml_document::save(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream, const char_t* indent, unsigned int flags) const
    {
        xml_writer_stream writer(stream);

        save(writer, indent, flags, encoding_wchar);
    }
#endif

    PUGI__FN bool xml_document::save_file(const char* path_, const char_t* indent, unsigned int flags, xml_encoding encoding) const
    {
        FILE* file = fopen(path_, (flags & format_save_file_text) ? "w" : "wb");
        return impl::save_file_impl(*this, file, indent, flags, encoding);
    }

    PUGI__FN bool xml_document::save_file(const wchar_t* path_, const char_t* indent, unsigned int flags, xml_encoding encoding) const
    {
        FILE* file = impl::open_file_wide(path_, (flags & format_save_file_text) ? L"w" : L"wb");
        return impl::save_file_impl(*this, file, indent, flags, encoding);
    }

    PUGI__FN xml_node xml_document::document_element() const
    {
        assert(_root);

        for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
            if (PUGI__NODETYPE(i) == node_element)
                return xml_node(i);

        return xml_node();
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN std::string PUGIXML_FUNCTION as_utf8(const wchar_t* str)
    {
        assert(str);

        return impl::as_utf8_impl(str, impl::strlength_wide(str));
    }

    PUGI__FN std::string PUGIXML_FUNCTION as_utf8(const std::basic_string<wchar_t>& str)
    {
        return impl::as_utf8_impl(str.c_str(), str.size());
    }
    
    PUGI__FN std::basic_string<wchar_t> PUGIXML_FUNCTION as_wide(const char* str)
    {
        assert(str);

        return impl::as_wide_impl(str, strlen(str));
    }
    
    PUGI__FN std::basic_string<wchar_t> PUGIXML_FUNCTION as_wide(const std::string& str)
    {
        return impl::as_wide_impl(str.c_str(), str.size());
    }
#endif

    PUGI__FN void PUGIXML_FUNCTION set_memory_management_functions(allocation_function allocate, deallocation_function deallocate)
    {
        impl::xml_memory::allocate = allocate;
        impl::xml_memory::deallocate = deallocate;
    }

    PUGI__FN allocation_function PUGIXML_FUNCTION get_memory_allocation_function()
    {
        return impl::xml_memory::allocate;
    }

    PUGI__FN deallocation_function PUGIXML_FUNCTION get_memory_deallocation_function()
    {
        return impl::xml_memory::deallocate;
    }
}

#if !defined(PUGIXML_NO_STL) && (defined(_MSC_VER) || defined(__ICC))
namespace std
{
    // Workarounds for (non-standard) iterator category detection for older versions (MSVC7/IC8 and earlier)
    PUGI__FN std::bidirectional_iterator_tag _Iter_cat(const pugi::xml_node_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }

    PUGI__FN std::bidirectional_iterator_tag _Iter_cat(const pugi::xml_attribute_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }

    PUGI__FN std::bidirectional_iterator_tag _Iter_cat(const pugi::xml_named_node_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }
}
#endif

#if !defined(PUGIXML_NO_STL) && defined(__SUNPRO_CC)
namespace std
{
    // Workarounds for (non-standard) iterator category detection
    PUGI__FN std::bidirectional_iterator_tag __iterator_category(const pugi::xml_node_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }

    PUGI__FN std::bidirectional_iterator_tag __iterator_category(const pugi::xml_attribute_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }

    PUGI__FN std::bidirectional_iterator_tag __iterator_category(const pugi::xml_named_node_iterator&)
    {
        return std::bidirectional_iterator_tag();
    }
}
#endif

#ifndef PUGIXML_NO_XPATH
// STL replacements
PUGI__NS_BEGIN
    struct equal_to
    {
        template <typename T> bool operator()(const T& lhs, const T& rhs) const
        {
            return lhs == rhs;
        }
    };

    struct not_equal_to
    {
        template <typename T> bool operator()(const T& lhs, const T& rhs) const
        {
            return lhs != rhs;
        }
    };

    struct less
    {
        template <typename T> bool operator()(const T& lhs, const T& rhs) const
        {
            return lhs < rhs;
        }
    };

    struct less_equal
    {
        template <typename T> bool operator()(const T& lhs, const T& rhs) const
        {
            return lhs <= rhs;
        }
    };

    template <typename T> void swap(T& lhs, T& rhs)
    {
        T temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    template <typename I, typename Pred> I min_element(I begin, I end, const Pred& pred)
    {
        I result = begin;

        for (I it = begin + 1; it != end; ++it)
            if (pred(*it, *result))
                result = it;

        return result;
    }

    template <typename I> void reverse(I begin, I end)
    {
        while (end - begin > 1) swap(*begin++, *--end);
    }

    template <typename I> I unique(I begin, I end)
    {
        // fast skip head
        while (end - begin > 1 && *begin != *(begin + 1)) begin++;

        if (begin == end) return begin;

        // last written element
        I write = begin++; 

        // merge unique elements
        while (begin != end)
        {
            if (*begin != *write)
                *++write = *begin++;
            else
                begin++;
        }

        // past-the-end (write points to live element)
        return write + 1;
    }

    template <typename I> void copy_backwards(I begin, I end, I target)
    {
        while (begin != end) *--target = *--end;
    }

    template <typename I, typename Pred, typename T> void insertion_sort(I begin, I end, const Pred& pred, T*)
    {
        assert(begin != end);

        for (I it = begin + 1; it != end; ++it)
        {
            T val = *it;

            if (pred(val, *begin))
            {
                // move to front
                copy_backwards(begin, it, it + 1);
                *begin = val;
            }
            else
            {
                I hole = it;

                // move hole backwards
                while (pred(val, *(hole - 1)))
                {
                    *hole = *(hole - 1);
                    hole--;
                }

                // fill hole with element
                *hole = val;
            }
        }
    }

    // std variant for elements with ==
    template <typename I, typename Pred> void partition(I begin, I middle, I end, const Pred& pred, I* out_eqbeg, I* out_eqend)
    {
        I eqbeg = middle, eqend = middle + 1;

        // expand equal range
        while (eqbeg != begin && *(eqbeg - 1) == *eqbeg) --eqbeg;
        while (eqend != end && *eqend == *eqbeg) ++eqend;

        // process outer elements
        I ltend = eqbeg, gtbeg = eqend;

        for (;;)
        {
            // find the element from the right side that belongs to the left one
            for (; gtbeg != end; ++gtbeg)
                if (!pred(*eqbeg, *gtbeg))
                {
                    if (*gtbeg == *eqbeg) swap(*gtbeg, *eqend++);
                    else break;
                }

            // find the element from the left side that belongs to the right one
            for (; ltend != begin; --ltend)
                if (!pred(*(ltend - 1), *eqbeg))
                {
                    if (*eqbeg == *(ltend - 1)) swap(*(ltend - 1), *--eqbeg);
                    else break;
                }

            // scanned all elements
            if (gtbeg == end && ltend == begin)
            {
                *out_eqbeg = eqbeg;
                *out_eqend = eqend;
                return;
            }

            // make room for elements by moving equal area
            if (gtbeg == end)
            {
                if (--ltend != --eqbeg) swap(*ltend, *eqbeg);
                swap(*eqbeg, *--eqend);
            }
            else if (ltend == begin)
            {
                if (eqend != gtbeg) swap(*eqbeg, *eqend);
                ++eqend;
                swap(*gtbeg++, *eqbeg++);
            }
            else swap(*gtbeg++, *--ltend);
        }
    }

    template <typename I, typename Pred> void median3(I first, I middle, I last, const Pred& pred)
    {
        if (pred(*middle, *first)) swap(*middle, *first);
        if (pred(*last, *middle)) swap(*last, *middle);
        if (pred(*middle, *first)) swap(*middle, *first);
    }

    template <typename I, typename Pred> void median(I first, I middle, I last, const Pred& pred)
    {
        if (last - first <= 40)
        {
            // median of three for small chunks
            median3(first, middle, last, pred);
        }
        else
        {
            // median of nine
            size_t step = (last - first + 1) / 8;

            median3(first, first + step, first + 2 * step, pred);
            median3(middle - step, middle, middle + step, pred);
            median3(last - 2 * step, last - step, last, pred);
            median3(first + step, middle, last - step, pred);
        }
    }

    template <typename I, typename Pred> void sort(I begin, I end, const Pred& pred)
    {
        // sort large chunks
        while (end - begin > 32)
        {
            // find median element
            I middle = begin + (end - begin) / 2;
            median(begin, middle, end - 1, pred);

            // partition in three chunks (< = >)
            I eqbeg, eqend;
            partition(begin, middle, end, pred, &eqbeg, &eqend);

            // loop on larger half
            if (eqbeg - begin > end - eqend)
            {
                sort(eqend, end, pred);
                end = eqbeg;
            }
            else
            {
                sort(begin, eqbeg, pred);
                begin = eqend;
            }
        }

        // insertion sort small chunk
        if (begin != end) insertion_sort(begin, end, pred, &*begin);
    }
PUGI__NS_END

// Allocator used for AST and evaluation stacks
PUGI__NS_BEGIN
    struct xpath_memory_block
    {    
        xpath_memory_block* next;
        size_t capacity;

        char data[
    #ifdef PUGIXML_MEMORY_XPATH_PAGE_SIZE
            PUGIXML_MEMORY_XPATH_PAGE_SIZE
    #else
            4096
    #endif
        ];
    };
        
    class xpath_allocator
    {
        xpath_memory_block* _root;
        size_t _root_size;

    public:
    #ifdef PUGIXML_NO_EXCEPTIONS
        jmp_buf* error_handler;
    #endif

        xpath_allocator(xpath_memory_block* root, size_t root_size = 0): _root(root), _root_size(root_size)
        {
        #ifdef PUGIXML_NO_EXCEPTIONS
            error_handler = 0;
        #endif
        }
        
        void* allocate_nothrow(size_t size)
        {
            // align size so that we're able to store pointers in subsequent blocks
            size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

            if (_root_size + size <= _root->capacity)
            {
                void* buf = _root->data + _root_size;
                _root_size += size;
                return buf;
            }
            else
            {
                // make sure we have at least 1/4th of the page free after allocation to satisfy subsequent allocation requests
                size_t block_capacity_base = sizeof(_root->data);
                size_t block_capacity_req = size + block_capacity_base / 4;
                size_t block_capacity = (block_capacity_base > block_capacity_req) ? block_capacity_base : block_capacity_req;

                size_t block_size = block_capacity + offsetof(xpath_memory_block, data);

                xpath_memory_block* block = static_cast<xpath_memory_block*>(xml_memory::allocate(block_size));
                if (!block) return 0;
                
                block->next = _root;
                block->capacity = block_capacity;
                
                _root = block;
                _root_size = size;
                
                return block->data;
            }
        }

        void* allocate(size_t size)
        {
            void* result = allocate_nothrow(size);

            if (!result)
            {
            #ifdef PUGIXML_NO_EXCEPTIONS
                assert(error_handler);
                longjmp(*error_handler, 1);
            #else
                throw std::bad_alloc();
            #endif
            }

            return result;
        }

        void* reallocate(void* ptr, size_t old_size, size_t new_size)
        {
            // align size so that we're able to store pointers in subsequent blocks
            old_size = (old_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
            new_size = (new_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

            // we can only reallocate the last object
            assert(ptr == 0 || static_cast<char*>(ptr) + old_size == _root->data + _root_size);

            // adjust root size so that we have not allocated the object at all
            bool only_object = (_root_size == old_size);

            if (ptr) _root_size -= old_size;

            // allocate a new version (this will obviously reuse the memory if possible)
            void* result = allocate(new_size);
            assert(result);

            // we have a new block
            if (result != ptr && ptr)
            {
                // copy old data
                assert(new_size >= old_size);
                memcpy(result, ptr, old_size);

                // free the previous page if it had no other objects
                if (only_object)
                {
                    assert(_root->data == result);
                    assert(_root->next);

                    xpath_memory_block* next = _root->next->next;

                    if (next)
                    {
                        // deallocate the whole page, unless it was the first one
                        xml_memory::deallocate(_root->next);
                        _root->next = next;
                    }
                }
            }

            return result;
        }

        void revert(const xpath_allocator& state)
        {
            // free all new pages
            xpath_memory_block* cur = _root;

            while (cur != state._root)
            {
                xpath_memory_block* next = cur->next;

                xml_memory::deallocate(cur);

                cur = next;
            }

            // restore state
            _root = state._root;
            _root_size = state._root_size;
        }

        void release()
        {
            xpath_memory_block* cur = _root;
            assert(cur);

            while (cur->next)
            {
                xpath_memory_block* next = cur->next;

                xml_memory::deallocate(cur);

                cur = next;
            }
        }
    };

    struct xpath_allocator_capture
    {
        xpath_allocator_capture(xpath_allocator* alloc): _target(alloc), _state(*alloc)
        {
        }

        ~xpath_allocator_capture()
        {
            _target->revert(_state);
        }

        xpath_allocator* _target;
        xpath_allocator _state;
    };

    struct xpath_stack
    {
        xpath_allocator* result;
        xpath_allocator* temp;
    };

    struct xpath_stack_data
    {
        xpath_memory_block blocks[2];
        xpath_allocator result;
        xpath_allocator temp;
        xpath_stack stack;

    #ifdef PUGIXML_NO_EXCEPTIONS
        jmp_buf error_handler;
    #endif

        xpath_stack_data(): result(blocks + 0), temp(blocks + 1)
        {
            blocks[0].next = blocks[1].next = 0;
            blocks[0].capacity = blocks[1].capacity = sizeof(blocks[0].data);

            stack.result = &result;
            stack.temp = &temp;

        #ifdef PUGIXML_NO_EXCEPTIONS
            result.error_handler = temp.error_handler = &error_handler;
        #endif
        }

        ~xpath_stack_data()
        {
            result.release();
            temp.release();
        }
    };
PUGI__NS_END

// String class
PUGI__NS_BEGIN
    class xpath_string
    {
        const char_t* _buffer;
        bool _uses_heap;
        size_t _length_heap;

        static char_t* duplicate_string(const char_t* string, size_t length, xpath_allocator* alloc)
        {
            char_t* result = static_cast<char_t*>(alloc->allocate((length + 1) * sizeof(char_t)));
            assert(result);

            memcpy(result, string, length * sizeof(char_t));
            result[length] = 0;

            return result;
        }

        xpath_string(const char_t* buffer, bool uses_heap_, size_t length_heap): _buffer(buffer), _uses_heap(uses_heap_), _length_heap(length_heap)
        {
        }

    public:
        static xpath_string from_const(const char_t* str)
        {
            return xpath_string(str, false, 0);
        }

        static xpath_string from_heap_preallocated(const char_t* begin, const char_t* end)
        {
            assert(begin <= end && *end == 0);

            return xpath_string(begin, true, static_cast<size_t>(end - begin));
        }

        static xpath_string from_heap(const char_t* begin, const char_t* end, xpath_allocator* alloc)
        {
            assert(begin <= end);

            size_t length = static_cast<size_t>(end - begin);

            return length == 0 ? xpath_string() : xpath_string(duplicate_string(begin, length, alloc), true, length);
        }

        xpath_string(): _buffer(PUGIXML_TEXT("")), _uses_heap(false), _length_heap(0)
        {
        }

        void append(const xpath_string& o, xpath_allocator* alloc)
        {
            // skip empty sources
            if (!*o._buffer) return;

            // fast append for constant empty target and constant source
            if (!*_buffer && !_uses_heap && !o._uses_heap)
            {
                _buffer = o._buffer;
            }
            else
            {
                // need to make heap copy
                size_t target_length = length();
                size_t source_length = o.length();
                size_t result_length = target_length + source_length;

                // allocate new buffer
                char_t* result = static_cast<char_t*>(alloc->reallocate(_uses_heap ? const_cast<char_t*>(_buffer) : 0, (target_length + 1) * sizeof(char_t), (result_length + 1) * sizeof(char_t)));
                assert(result);

                // append first string to the new buffer in case there was no reallocation
                if (!_uses_heap) memcpy(result, _buffer, target_length * sizeof(char_t));

                // append second string to the new buffer
                memcpy(result + target_length, o._buffer, source_length * sizeof(char_t));
                result[result_length] = 0;

                // finalize
                _buffer = result;
                _uses_heap = true;
                _length_heap = result_length;
            }
        }

        const char_t* c_str() const
        {
            return _buffer;
        }

        size_t length() const
        {
            return _uses_heap ? _length_heap : strlength(_buffer);
        }
        
        char_t* data(xpath_allocator* alloc)
        {
            // make private heap copy
            if (!_uses_heap)
            {
                size_t length_ = strlength(_buffer);

                _buffer = duplicate_string(_buffer, length_, alloc);
                _uses_heap = true;
                _length_heap = length_;
            }

            return const_cast<char_t*>(_buffer);
        }

        bool empty() const
        {
            return *_buffer == 0;
        }

        bool operator==(const xpath_string& o) const
        {
            return strequal(_buffer, o._buffer);
        }

        bool operator!=(const xpath_string& o) const
        {
            return !strequal(_buffer, o._buffer);
        }

        bool uses_heap() const
        {
            return _uses_heap;
        }
    };
PUGI__NS_END

PUGI__NS_BEGIN
    PUGI__FN bool starts_with(const char_t* string, const char_t* pattern)
    {
        while (*pattern && *string == *pattern)
        {
            string++;
            pattern++;
        }

        return *pattern == 0;
    }

    PUGI__FN const char_t* find_char(const char_t* s, char_t c)
    {
    #ifdef PUGIXML_WCHAR_MODE
        return wcschr(s, c);
    #else
        return strchr(s, c);
    #endif
    }

    PUGI__FN const char_t* find_substring(const char_t* s, const char_t* p)
    {
    #ifdef PUGIXML_WCHAR_MODE
        // MSVC6 wcsstr bug workaround (if s is empty it always returns 0)
        return (*p == 0) ? s : wcsstr(s, p);
    #else
        return strstr(s, p);
    #endif
    }

    // Converts symbol to lower case, if it is an ASCII one
    PUGI__FN char_t tolower_ascii(char_t ch)
    {
        return static_cast<unsigned int>(ch - 'A') < 26 ? static_cast<char_t>(ch | ' ') : ch;
    }

    PUGI__FN xpath_string string_value(const xpath_node& na, xpath_allocator* alloc)
    {
        if (na.attribute())
            return xpath_string::from_const(na.attribute().value());
        else
        {
            xml_node n = na.node();

            switch (n.type())
            {
            case node_pcdata:
            case node_cdata:
            case node_comment:
            case node_pi:
                return xpath_string::from_const(n.value());
            
            case node_document:
            case node_element:
            {
                xpath_string result;

                xml_node cur = n.first_child();
                
                while (cur && cur != n)
                {
                    if (cur.type() == node_pcdata || cur.type() == node_cdata)
                        result.append(xpath_string::from_const(cur.value()), alloc);

                    if (cur.first_child())
                        cur = cur.first_child();
                    else if (cur.next_sibling())
                        cur = cur.next_sibling();
                    else
                    {
                        while (!cur.next_sibling() && cur != n)
                            cur = cur.parent();

                        if (cur != n) cur = cur.next_sibling();
                    }
                }
                
                return result;
            }
            
            default:
                return xpath_string();
            }
        }
    }
    
    PUGI__FN bool node_is_before_sibling(xml_node_struct* ln, xml_node_struct* rn)
    {
        assert(ln->parent == rn->parent);

        // there is no common ancestor (the shared parent is null), nodes are from different documents
        if (!ln->parent) return ln < rn;

        // determine sibling order
        xml_node_struct* ls = ln;
        xml_node_struct* rs = rn;

        while (ls && rs)
        {
            if (ls == rn) return true;
            if (rs == ln) return false;

            ls = ls->next_sibling;
            rs = rs->next_sibling;
        }

        // if rn sibling chain ended ln must be before rn
        return !rs;
    }
    
    PUGI__FN bool node_is_before(xml_node_struct* ln, xml_node_struct* rn)
    {
        // find common ancestor at the same depth, if any
        xml_node_struct* lp = ln;
        xml_node_struct* rp = rn;

        while (lp && rp && lp->parent != rp->parent)
        {
            lp = lp->parent;
            rp = rp->parent;
        }

        // parents are the same!
        if (lp && rp) return node_is_before_sibling(lp, rp);

        // nodes are at different depths, need to normalize heights
        bool left_higher = !lp;

        while (lp)
        {
            lp = lp->parent;
            ln = ln->parent;
        }

        while (rp)
        {
            rp = rp->parent;
            rn = rn->parent;
        }

        // one node is the ancestor of the other
        if (ln == rn) return left_higher;

        // find common ancestor... again
        while (ln->parent != rn->parent)
        {
            ln = ln->parent;
            rn = rn->parent;
        }

        return node_is_before_sibling(ln, rn);
    }

    PUGI__FN bool node_is_ancestor(xml_node_struct* parent, xml_node_struct* node)
    {
        while (node && node != parent) node = node->parent;

        return parent && node == parent;
    }

    PUGI__FN const void* document_buffer_order(const xpath_node& xnode)
    {
        xml_node_struct* node = xnode.node().internal_object();

        if (node)
        {
            if ((get_document(node).header & xml_memory_page_contents_shared_mask) == 0)
            {
                if (node->name && (node->header & impl::xml_memory_page_name_allocated_or_shared_mask) == 0) return node->name;
                if (node->value && (node->header & impl::xml_memory_page_value_allocated_or_shared_mask) == 0) return node->value;
            }

            return 0;
        }

        xml_attribute_struct* attr = xnode.attribute().internal_object();

        if (attr)
        {
            if ((get_document(attr).header & xml_memory_page_contents_shared_mask) == 0)
            {
                if ((attr->header & impl::xml_memory_page_name_allocated_or_shared_mask) == 0) return attr->name;
                if ((attr->header & impl::xml_memory_page_value_allocated_or_shared_mask) == 0) return attr->value;
            }

            return 0;
        }

        return 0;
    }
    
    struct document_order_comparator
    {
        bool operator()(const xpath_node& lhs, const xpath_node& rhs) const
        {
            // optimized document order based check
            const void* lo = document_buffer_order(lhs);
            const void* ro = document_buffer_order(rhs);

            if (lo && ro) return lo < ro;

            // slow comparison
            xml_node ln = lhs.node(), rn = rhs.node();

            // compare attributes
            if (lhs.attribute() && rhs.attribute())
            {
                // shared parent
                if (lhs.parent() == rhs.parent())
                {
                    // determine sibling order
                    for (xml_attribute a = lhs.attribute(); a; a = a.next_attribute())
                        if (a == rhs.attribute())
                            return true;
                    
                    return false;
                }
                
                // compare attribute parents
                ln = lhs.parent();
                rn = rhs.parent();
            }
            else if (lhs.attribute())
            {
                // attributes go after the parent element
                if (lhs.parent() == rhs.node()) return false;
                
                ln = lhs.parent();
            }
            else if (rhs.attribute())
            {
                // attributes go after the parent element
                if (rhs.parent() == lhs.node()) return true;
                
                rn = rhs.parent();
            }

            if (ln == rn) return false;

            if (!ln || !rn) return ln < rn;
            
            return node_is_before(ln.internal_object(), rn.internal_object());
        }
    };

    struct duplicate_comparator
    {
        bool operator()(const xpath_node& lhs, const xpath_node& rhs) const
        {
            if (lhs.attribute()) return rhs.attribute() ? lhs.attribute() < rhs.attribute() : true;
            else return rhs.attribute() ? false : lhs.node() < rhs.node();
        }
    };
    
    PUGI__FN double gen_nan()
    {
    #if defined(__STDC_IEC_559__) || ((FLT_RADIX - 0 == 2) && (FLT_MAX_EXP - 0 == 128) && (FLT_MANT_DIG - 0 == 24))
        union { float f; uint32_t i; } u[sizeof(float) == sizeof(uint32_t) ? 1 : -1];
        u[0].i = 0x7fc00000;
        return u[0].f;
    #else
        // fallback
        const volatile double zero = 0.0;
        return zero / zero;
    #endif
    }
    
    PUGI__FN bool is_nan(double value)
    {
    #if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__)
        return !!_isnan(value);
    #elif defined(fpclassify) && defined(FP_NAN)
        return fpclassify(value) == FP_NAN;
    #else
        // fallback
        const volatile double v = value;
        return v != v;
    #endif
    }
    
    PUGI__FN const char_t* convert_number_to_string_special(double value)
    {
    #if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__)
        if (_finite(value)) return (value == 0) ? PUGIXML_TEXT("0") : 0;
        if (_isnan(value)) return PUGIXML_TEXT("NaN");
        return value > 0 ? PUGIXML_TEXT("Infinity") : PUGIXML_TEXT("-Infinity");
    #elif defined(fpclassify) && defined(FP_NAN) && defined(FP_INFINITE) && defined(FP_ZERO)
        switch (fpclassify(value))
        {
        case FP_NAN:
            return PUGIXML_TEXT("NaN");

        case FP_INFINITE:
            return value > 0 ? PUGIXML_TEXT("Infinity") : PUGIXML_TEXT("-Infinity");

        case FP_ZERO:
            return PUGIXML_TEXT("0");

        default:
            return 0;
        }
    #else
        // fallback
        const volatile double v = value;

        if (v == 0) return PUGIXML_TEXT("0");
        if (v != v) return PUGIXML_TEXT("NaN");
        if (v * 2 == v) return value > 0 ? PUGIXML_TEXT("Infinity") : PUGIXML_TEXT("-Infinity");
        return 0;
    #endif
    }
    
    PUGI__FN bool convert_number_to_boolean(double value)
    {
        return (value != 0 && !is_nan(value));
    }
    
    PUGI__FN void truncate_zeros(char* begin, char* end)
    {
        while (begin != end && end[-1] == '0') end--;

        *end = 0;
    }

    // gets mantissa digits in the form of 0.xxxxx with 0. implied and the exponent
#if defined(PUGI__MSVC_CRT_VERSION) && PUGI__MSVC_CRT_VERSION >= 1400 && !defined(_WIN32_WCE)
    PUGI__FN void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
    {
        // get base values
        int sign, exponent;
        _ecvt_s(buffer, buffer_size, value, DBL_DIG + 1, &exponent, &sign);

        // truncate redundant zeros
        truncate_zeros(buffer, buffer + strlen(buffer));

        // fill results
        *out_mantissa = buffer;
        *out_exponent = exponent;
    }
#else
    PUGI__FN void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
    {
        // get a scientific notation value with IEEE DBL_DIG decimals
        sprintf(buffer, "%.*e", DBL_DIG, value);
        assert(strlen(buffer) < buffer_size);
        (void)!buffer_size;

        // get the exponent (possibly negative)
        char* exponent_string = strchr(buffer, 'e');
        assert(exponent_string);

        int exponent = atoi(exponent_string + 1);

        // extract mantissa string: skip sign
        char* mantissa = buffer[0] == '-' ? buffer + 1 : buffer;
        assert(mantissa[0] != '0' && mantissa[1] == '.');

        // divide mantissa by 10 to eliminate integer part
        mantissa[1] = mantissa[0];
        mantissa++;
        exponent++;

        // remove extra mantissa digits and zero-terminate mantissa
        truncate_zeros(mantissa, exponent_string);

        // fill results
        *out_mantissa = mantissa;
        *out_exponent = exponent;
    }
#endif

    PUGI__FN xpath_string convert_number_to_string(double value, xpath_allocator* alloc)
    {
        // try special number conversion
        const char_t* special = convert_number_to_string_special(value);
        if (special) return xpath_string::from_const(special);

        // get mantissa + exponent form
        char mantissa_buffer[32];

        char* mantissa;
        int exponent;
        convert_number_to_mantissa_exponent(value, mantissa_buffer, sizeof(mantissa_buffer), &mantissa, &exponent);

        // allocate a buffer of suitable length for the number
        size_t result_size = strlen(mantissa_buffer) + (exponent > 0 ? exponent : -exponent) + 4;
        char_t* result = static_cast<char_t*>(alloc->allocate(sizeof(char_t) * result_size));
        assert(result);

        // make the number!
        char_t* s = result;

        // sign
        if (value < 0) *s++ = '-';

        // integer part
        if (exponent <= 0)
        {
            *s++ = '0';
        }
        else
        {
            while (exponent > 0)
            {
                assert(*mantissa == 0 || static_cast<unsigned int>(static_cast<unsigned int>(*mantissa) - '0') <= 9);
                *s++ = *mantissa ? *mantissa++ : '0';
                exponent--;
            }
        }

        // fractional part
        if (*mantissa)
        {
            // decimal point
            *s++ = '.';

            // extra zeroes from negative exponent
            while (exponent < 0)
            {
                *s++ = '0';
                exponent++;
            }

            // extra mantissa digits
            while (*mantissa)
            {
                assert(static_cast<unsigned int>(*mantissa - '0') <= 9);
                *s++ = *mantissa++;
            }
        }

        // zero-terminate
        assert(s < result + result_size);
        *s = 0;

        return xpath_string::from_heap_preallocated(result, s);
    }
    
    PUGI__FN bool check_string_to_number_format(const char_t* string)
    {
        // parse leading whitespace
        while (PUGI__IS_CHARTYPE(*string, ct_space)) ++string;

        // parse sign
        if (*string == '-') ++string;

        if (!*string) return false;

        // if there is no integer part, there should be a decimal part with at least one digit
        if (!PUGI__IS_CHARTYPEX(string[0], ctx_digit) && (string[0] != '.' || !PUGI__IS_CHARTYPEX(string[1], ctx_digit))) return false;

        // parse integer part
        while (PUGI__IS_CHARTYPEX(*string, ctx_digit)) ++string;

        // parse decimal part
        if (*string == '.')
        {
            ++string;

            while (PUGI__IS_CHARTYPEX(*string, ctx_digit)) ++string;
        }

        // parse trailing whitespace
        while (PUGI__IS_CHARTYPE(*string, ct_space)) ++string;

        return *string == 0;
    }

    PUGI__FN double convert_string_to_number(const char_t* string)
    {
        // check string format
        if (!check_string_to_number_format(string)) return gen_nan();

        // parse string
    #ifdef PUGIXML_WCHAR_MODE
        return wcstod(string, 0);
    #else
        return atof(string);
    #endif
    }

    PUGI__FN bool convert_string_to_number_scratch(char_t (&buffer)[32], const char_t* begin, const char_t* end, double* out_result)
    {
        size_t length = static_cast<size_t>(end - begin);
        char_t* scratch = buffer;

        if (length >= sizeof(buffer) / sizeof(buffer[0]))
        {
            // need to make dummy on-heap copy
            scratch = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
            if (!scratch) return false;
        }

        // copy string to zero-terminated buffer and perform conversion
        memcpy(scratch, begin, length * sizeof(char_t));
        scratch[length] = 0;

        *out_result = convert_string_to_number(scratch);

        // free dummy buffer
        if (scratch != buffer) xml_memory::deallocate(scratch);

        return true;
    }
    
    PUGI__FN double round_nearest(double value)
    {
        return floor(value + 0.5);
    }

    PUGI__FN double round_nearest_nzero(double value)
    {
        // same as round_nearest, but returns -0 for [-0.5, -0]
        // ceil is used to differentiate between +0 and -0 (we return -0 for [-0.5, -0] and +0 for +0)
        return (value >= -0.5 && value <= 0) ? ceil(value) : floor(value + 0.5);
    }
    
    PUGI__FN const char_t* qualified_name(const xpath_node& node)
    {
        return node.attribute() ? node.attribute().name() : node.node().name();
    }
    
    PUGI__FN const char_t* local_name(const xpath_node& node)
    {
        const char_t* name = qualified_name(node);
        const char_t* p = find_char(name, ':');
        
        return p ? p + 1 : name;
    }

    struct namespace_uri_predicate
    {
        const char_t* prefix;
        size_t prefix_length;

        namespace_uri_predicate(const char_t* name)
        {
            const char_t* pos = find_char(name, ':');

            prefix = pos ? name : 0;
            prefix_length = pos ? static_cast<size_t>(pos - name) : 0;
        }

        bool operator()(xml_attribute a) const
        {
            const char_t* name = a.name();

            if (!starts_with(name, PUGIXML_TEXT("xmlns"))) return false;

            return prefix ? name[5] == ':' && strequalrange(name + 6, prefix, prefix_length) : name[5] == 0;
        }
    };

    PUGI__FN const char_t* namespace_uri(xml_node node)
    {
        namespace_uri_predicate pred = node.name();
        
        xml_node p = node;
        
        while (p)
        {
            xml_attribute a = p.find_attribute(pred);
            
            if (a) return a.value();
            
            p = p.parent();
        }
        
        return PUGIXML_TEXT("");
    }

    PUGI__FN const char_t* namespace_uri(xml_attribute attr, xml_node parent)
    {
        namespace_uri_predicate pred = attr.name();
        
        // Default namespace does not apply to attributes
        if (!pred.prefix) return PUGIXML_TEXT("");
        
        xml_node p = parent;
        
        while (p)
        {
            xml_attribute a = p.find_attribute(pred);
            
            if (a) return a.value();
            
            p = p.parent();
        }
        
        return PUGIXML_TEXT("");
    }

    PUGI__FN const char_t* namespace_uri(const xpath_node& node)
    {
        return node.attribute() ? namespace_uri(node.attribute(), node.parent()) : namespace_uri(node.node());
    }

    PUGI__FN char_t* normalize_space(char_t* buffer)
    {
        char_t* write = buffer;

        for (char_t* it = buffer; *it; )
        {
            char_t ch = *it++;

            if (PUGI__IS_CHARTYPE(ch, ct_space))
            {
                // replace whitespace sequence with single space
                while (PUGI__IS_CHARTYPE(*it, ct_space)) it++;

                // avoid leading spaces
                if (write != buffer) *write++ = ' ';
            }
            else *write++ = ch;
        }

        // remove trailing space
        if (write != buffer && PUGI__IS_CHARTYPE(write[-1], ct_space)) write--;

        // zero-terminate
        *write = 0;

        return write;
    }

    PUGI__FN char_t* translate(char_t* buffer, const char_t* from, const char_t* to, size_t to_length)
    {
        char_t* write = buffer;

        while (*buffer)
        {
            PUGI__DMC_VOLATILE char_t ch = *buffer++;

            const char_t* pos = find_char(from, ch);

            if (!pos)
                *write++ = ch; // do not process
            else if (static_cast<size_t>(pos - from) < to_length)
                *write++ = to[pos - from]; // replace
        }

        // zero-terminate
        *write = 0;

        return write;
    }

    PUGI__FN unsigned char* translate_table_generate(xpath_allocator* alloc, const char_t* from, const char_t* to)
    {
        unsigned char table[128] = {0};

        while (*from)
        {
            unsigned int fc = static_cast<unsigned int>(*from);
            unsigned int tc = static_cast<unsigned int>(*to);

            if (fc >= 128 || tc >= 128)
                return 0;

            // code=128 means "skip character"
            if (!table[fc])
                table[fc] = static_cast<unsigned char>(tc ? tc : 128);

            from++;
            if (tc) to++;
        }

        for (int i = 0; i < 128; ++i)
            if (!table[i])
                table[i] = static_cast<unsigned char>(i);

        void* result = alloc->allocate_nothrow(sizeof(table));

        if (result)
        {
            memcpy(result, table, sizeof(table));
        }

        return static_cast<unsigned char*>(result);
    }

    PUGI__FN char_t* translate_table(char_t* buffer, const unsigned char* table)
    {
        char_t* write = buffer;

        while (*buffer)
        {
            char_t ch = *buffer++;
            unsigned int index = static_cast<unsigned int>(ch);

            if (index < 128)
            {
                unsigned char code = table[index];

                // code=128 means "skip character" (table size is 128 so 128 can be a special value)
                // this code skips these characters without extra branches
                *write = static_cast<char_t>(code);
                write += 1 - (code >> 7);
            }
            else
            {
                *write++ = ch;
            }
        }

        // zero-terminate
        *write = 0;

        return write;
    }

    inline bool is_xpath_attribute(const char_t* name)
    {
        return !(starts_with(name, PUGIXML_TEXT("xmlns")) && (name[5] == 0 || name[5] == ':'));
    }

    struct xpath_variable_boolean: xpath_variable
    {
        xpath_variable_boolean(): value(false)
        {
        }

        bool value;
        char_t name[1];
    };

    struct xpath_variable_number: xpath_variable
    {
        xpath_variable_number(): value(0)
        {
        }

        double value;
        char_t name[1];
    };

    struct xpath_variable_string: xpath_variable
    {
        xpath_variable_string(): value(0)
        {
        }

        ~xpath_variable_string()
        {
            if (value) xml_memory::deallocate(value);
        }

        char_t* value;
        char_t name[1];
    };

    struct xpath_variable_node_set: xpath_variable
    {
        xpath_node_set value;
        char_t name[1];
    };

    static const xpath_node_set dummy_node_set;

    PUGI__FN unsigned int hash_string(const char_t* str)
    {
        // Jenkins one-at-a-time hash (http://en.wikipedia.org/wiki/Jenkins_hash_function#one-at-a-time)
        unsigned int result = 0;

        while (*str)
        {
            result += static_cast<unsigned int>(*str++);
            result += result << 10;
            result ^= result >> 6;
        }
    
        result += result << 3;
        result ^= result >> 11;
        result += result << 15;
    
        return result;
    }

    template <typename T> PUGI__FN T* new_xpath_variable(const char_t* name)
    {
        size_t length = strlength(name);
        if (length == 0) return 0; // empty variable names are invalid

        // $$ we can't use offsetof(T, name) because T is non-POD, so we just allocate additional length characters
        void* memory = xml_memory::allocate(sizeof(T) + length * sizeof(char_t));
        if (!memory) return 0;

        T* result = new (memory) T();

        memcpy(result->name, name, (length + 1) * sizeof(char_t));

        return result;
    }

    PUGI__FN xpath_variable* new_xpath_variable(xpath_value_type type, const char_t* name)
    {
        switch (type)
        {
        case xpath_type_node_set:
            return new_xpath_variable<xpath_variable_node_set>(name);

        case xpath_type_number:
            return new_xpath_variable<xpath_variable_number>(name);

        case xpath_type_string:
            return new_xpath_variable<xpath_variable_string>(name);

        case xpath_type_boolean:
            return new_xpath_variable<xpath_variable_boolean>(name);

        default:
            return 0;
        }
    }

    template <typename T> PUGI__FN void delete_xpath_variable(T* var)
    {
        var->~T();
        xml_memory::deallocate(var);
    }

    PUGI__FN void delete_xpath_variable(xpath_value_type type, xpath_variable* var)
    {
        switch (type)
        {
        case xpath_type_node_set:
            delete_xpath_variable(static_cast<xpath_variable_node_set*>(var));
            break;

        case xpath_type_number:
            delete_xpath_variable(static_cast<xpath_variable_number*>(var));
            break;

        case xpath_type_string:
            delete_xpath_variable(static_cast<xpath_variable_string*>(var));
            break;

        case xpath_type_boolean:
            delete_xpath_variable(static_cast<xpath_variable_boolean*>(var));
            break;

        default:
            assert(!"Invalid variable type");
        }
    }

    PUGI__FN xpath_variable* get_variable_scratch(char_t (&buffer)[32], xpath_variable_set* set, const char_t* begin, const char_t* end)
    {
        size_t length = static_cast<size_t>(end - begin);
        char_t* scratch = buffer;

        if (length >= sizeof(buffer) / sizeof(buffer[0]))
        {
            // need to make dummy on-heap copy
            scratch = static_cast<char_t*>(xml_memory::allocate((length + 1) * sizeof(char_t)));
            if (!scratch) return 0;
        }

        // copy string to zero-terminated buffer and perform lookup
        memcpy(scratch, begin, length * sizeof(char_t));
        scratch[length] = 0;

        xpath_variable* result = set->get(scratch);

        // free dummy buffer
        if (scratch != buffer) xml_memory::deallocate(scratch);

        return result;
    }
PUGI__NS_END

// Internal node set class
PUGI__NS_BEGIN
    PUGI__FN xpath_node_set::type_t xpath_get_order(const xpath_node* begin, const xpath_node* end)
    {
        if (end - begin < 2)
            return xpath_node_set::type_sorted;

        document_order_comparator cmp;

        bool first = cmp(begin[0], begin[1]);

        for (const xpath_node* it = begin + 1; it + 1 < end; ++it)
            if (cmp(it[0], it[1]) != first)
                return xpath_node_set::type_unsorted;

        return first ? xpath_node_set::type_sorted : xpath_node_set::type_sorted_reverse;
    }

    PUGI__FN xpath_node_set::type_t xpath_sort(xpath_node* begin, xpath_node* end, xpath_node_set::type_t type, bool rev)
    {
        xpath_node_set::type_t order = rev ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_sorted;

        if (type == xpath_node_set::type_unsorted)
        {
            xpath_node_set::type_t sorted = xpath_get_order(begin, end);

            if (sorted == xpath_node_set::type_unsorted)
            {
                sort(begin, end, document_order_comparator());

                type = xpath_node_set::type_sorted;
            }
            else
                type = sorted;
        }
        
        if (type != order) reverse(begin, end);
            
        return order;
    }

    PUGI__FN xpath_node xpath_first(const xpath_node* begin, const xpath_node* end, xpath_node_set::type_t type)
    {
        if (begin == end) return xpath_node();

        switch (type)
        {
        case xpath_node_set::type_sorted:
            return *begin;

        case xpath_node_set::type_sorted_reverse:
            return *(end - 1);

        case xpath_node_set::type_unsorted:
            return *min_element(begin, end, document_order_comparator());

        default:
            assert(!"Invalid node set type");
            return xpath_node();
        }
    }

    class xpath_node_set_raw
    {
        xpath_node_set::type_t _type;

        xpath_node* _begin;
        xpath_node* _end;
        xpath_node* _eos;

    public:
        xpath_node_set_raw(): _type(xpath_node_set::type_unsorted), _begin(0), _end(0), _eos(0)
        {
        }

        xpath_node* begin() const
        {
            return _begin;
        }

        xpath_node* end() const
        {
            return _end;
        }

        bool empty() const
        {
            return _begin == _end;
        }

        size_t size() const
        {
            return static_cast<size_t>(_end - _begin);
        }

        xpath_node first() const
        {
            return xpath_first(_begin, _end, _type);
        }

        void push_back_grow(const xpath_node& node, xpath_allocator* alloc);

        void push_back(const xpath_node& node, xpath_allocator* alloc)
        {
            if (_end != _eos)
                *_end++ = node;
            else
                push_back_grow(node, alloc);
        }

        void append(const xpath_node* begin_, const xpath_node* end_, xpath_allocator* alloc)
        {
            if (begin_ == end_) return;

            size_t size_ = static_cast<size_t>(_end - _begin);
            size_t capacity = static_cast<size_t>(_eos - _begin);
            size_t count = static_cast<size_t>(end_ - begin_);

            if (size_ + count > capacity)
            {
                // reallocate the old array or allocate a new one
                xpath_node* data = static_cast<xpath_node*>(alloc->reallocate(_begin, capacity * sizeof(xpath_node), (size_ + count) * sizeof(xpath_node)));
                assert(data);

                // finalize
                _begin = data;
                _end = data + size_;
                _eos = data + size_ + count;
            }

            memcpy(_end, begin_, count * sizeof(xpath_node));
            _end += count;
        }

        void sort_do()
        {
            _type = xpath_sort(_begin, _end, _type, false);
        }

        void truncate(xpath_node* pos)
        {
            assert(_begin <= pos && pos <= _end);

            _end = pos;
        }

        void remove_duplicates()
        {
            if (_type == xpath_node_set::type_unsorted)
                sort(_begin, _end, duplicate_comparator());
        
            _end = unique(_begin, _end);
        }

        xpath_node_set::type_t type() const
        {
            return _type;
        }

        void set_type(xpath_node_set::type_t value)
        {
            _type = value;
        }
    };

    PUGI__FN_NO_INLINE void xpath_node_set_raw::push_back_grow(const xpath_node& node, xpath_allocator* alloc)
    {
        size_t capacity = static_cast<size_t>(_eos - _begin);

        // get new capacity (1.5x rule)
        size_t new_capacity = capacity + capacity / 2 + 1;

        // reallocate the old array or allocate a new one
        xpath_node* data = static_cast<xpath_node*>(alloc->reallocate(_begin, capacity * sizeof(xpath_node), new_capacity * sizeof(xpath_node)));
        assert(data);

        // finalize
        _begin = data;
        _end = data + capacity;
        _eos = data + new_capacity;

        // push
        *_end++ = node;
    }
PUGI__NS_END

PUGI__NS_BEGIN
    struct xpath_context
    {
        xpath_node n;
        size_t position, size;

        xpath_context(const xpath_node& n_, size_t position_, size_t size_): n(n_), position(position_), size(size_)
        {
        }
    };

    enum lexeme_t
    {
        lex_none = 0,
        lex_equal,
        lex_not_equal,
        lex_less,
        lex_greater,
        lex_less_or_equal,
        lex_greater_or_equal,
        lex_plus,
        lex_minus,
        lex_multiply,
        lex_union,
        lex_var_ref,
        lex_open_brace,
        lex_close_brace,
        lex_quoted_string,
        lex_number,
        lex_slash,
        lex_double_slash,
        lex_open_square_brace,
        lex_close_square_brace,
        lex_string,
        lex_comma,
        lex_axis_attribute,
        lex_dot,
        lex_double_dot,
        lex_double_colon,
        lex_eof
    };

    struct xpath_lexer_string
    {
        const char_t* begin;
        const char_t* end;

        xpath_lexer_string(): begin(0), end(0)
        {
        }

        bool operator==(const char_t* other) const
        {
            size_t length = static_cast<size_t>(end - begin);

            return strequalrange(other, begin, length);
        }
    };

    class xpath_lexer
    {
        const char_t* _cur;
        const char_t* _cur_lexeme_pos;
        xpath_lexer_string _cur_lexeme_contents;

        lexeme_t _cur_lexeme;

    public:
        explicit xpath_lexer(const char_t* query): _cur(query)
        {
            next();
        }
        
        const char_t* state() const
        {
            return _cur;
        }
        
        void next()
        {
            const char_t* cur = _cur;

            while (PUGI__IS_CHARTYPE(*cur, ct_space)) ++cur;

            // save lexeme position for error reporting
            _cur_lexeme_pos = cur;

            switch (*cur)
            {
            case 0:
                _cur_lexeme = lex_eof;
                break;
            
            case '>':
                if (*(cur+1) == '=')
                {
                    cur += 2;
                    _cur_lexeme = lex_greater_or_equal;
                }
                else
                {
                    cur += 1;
                    _cur_lexeme = lex_greater;
                }
                break;

            case '<':
                if (*(cur+1) == '=')
                {
                    cur += 2;
                    _cur_lexeme = lex_less_or_equal;
                }
                else
                {
                    cur += 1;
                    _cur_lexeme = lex_less;
                }
                break;

            case '!':
                if (*(cur+1) == '=')
                {
                    cur += 2;
                    _cur_lexeme = lex_not_equal;
                }
                else
                {
                    _cur_lexeme = lex_none;
                }
                break;

            case '=':
                cur += 1;
                _cur_lexeme = lex_equal;

                break;
            
            case '+':
                cur += 1;
                _cur_lexeme = lex_plus;

                break;

            case '-':
                cur += 1;
                _cur_lexeme = lex_minus;

                break;

            case '*':
                cur += 1;
                _cur_lexeme = lex_multiply;

                break;

            case '|':
                cur += 1;
                _cur_lexeme = lex_union;

                break;
            
            case '$':
                cur += 1;

                if (PUGI__IS_CHARTYPEX(*cur, ctx_start_symbol))
                {
                    _cur_lexeme_contents.begin = cur;

                    while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;

                    if (cur[0] == ':' && PUGI__IS_CHARTYPEX(cur[1], ctx_symbol)) // qname
                    {
                        cur++; // :

                        while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;
                    }

                    _cur_lexeme_contents.end = cur;
                
                    _cur_lexeme = lex_var_ref;
                }
                else
                {
                    _cur_lexeme = lex_none;
                }

                break;

            case '(':
                cur += 1;
                _cur_lexeme = lex_open_brace;

                break;

            case ')':
                cur += 1;
                _cur_lexeme = lex_close_brace;

                break;
            
            case '[':
                cur += 1;
                _cur_lexeme = lex_open_square_brace;

                break;

            case ']':
                cur += 1;
                _cur_lexeme = lex_close_square_brace;

                break;

            case ',':
                cur += 1;
                _cur_lexeme = lex_comma;

                break;

            case '/':
                if (*(cur+1) == '/')
                {
                    cur += 2;
                    _cur_lexeme = lex_double_slash;
                }
                else
                {
                    cur += 1;
                    _cur_lexeme = lex_slash;
                }
                break;
        
            case '.':
                if (*(cur+1) == '.')
                {
                    cur += 2;
                    _cur_lexeme = lex_double_dot;
                }
                else if (PUGI__IS_CHARTYPEX(*(cur+1), ctx_digit))
                {
                    _cur_lexeme_contents.begin = cur; // .

                    ++cur;

                    while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;

                    _cur_lexeme_contents.end = cur;
                    
                    _cur_lexeme = lex_number;
                }
                else
                {
                    cur += 1;
                    _cur_lexeme = lex_dot;
                }
                break;

            case '@':
                cur += 1;
                _cur_lexeme = lex_axis_attribute;

                break;

            case '"':
            case '\'':
            {
                char_t terminator = *cur;

                ++cur;

                _cur_lexeme_contents.begin = cur;
                while (*cur && *cur != terminator) cur++;
                _cur_lexeme_contents.end = cur;
                
                if (!*cur)
                    _cur_lexeme = lex_none;
                else
                {
                    cur += 1;
                    _cur_lexeme = lex_quoted_string;
                }

                break;
            }

            case ':':
                if (*(cur+1) == ':')
                {
                    cur += 2;
                    _cur_lexeme = lex_double_colon;
                }
                else
                {
                    _cur_lexeme = lex_none;
                }
                break;

            default:
                if (PUGI__IS_CHARTYPEX(*cur, ctx_digit))
                {
                    _cur_lexeme_contents.begin = cur;

                    while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;
                
                    if (*cur == '.')
                    {
                        cur++;

                        while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;
                    }

                    _cur_lexeme_contents.end = cur;

                    _cur_lexeme = lex_number;
                }
                else if (PUGI__IS_CHARTYPEX(*cur, ctx_start_symbol))
                {
                    _cur_lexeme_contents.begin = cur;

                    while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;

                    if (cur[0] == ':')
                    {
                        if (cur[1] == '*') // namespace test ncname:*
                        {
                            cur += 2; // :*
                        }
                        else if (PUGI__IS_CHARTYPEX(cur[1], ctx_symbol)) // namespace test qname
                        {
                            cur++; // :

                            while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;
                        }
                    }

                    _cur_lexeme_contents.end = cur;
                
                    _cur_lexeme = lex_string;
                }
                else
                {
                    _cur_lexeme = lex_none;
                }
            }

            _cur = cur;
        }

        lexeme_t current() const
        {
            return _cur_lexeme;
        }

        const char_t* current_pos() const
        {
            return _cur_lexeme_pos;
        }

        const xpath_lexer_string& contents() const
        {
            assert(_cur_lexeme == lex_var_ref || _cur_lexeme == lex_number || _cur_lexeme == lex_string || _cur_lexeme == lex_quoted_string);

            return _cur_lexeme_contents;
        }
    };

    enum ast_type_t
    {
        ast_unknown,
        ast_op_or,                        // left or right
        ast_op_and,                        // left and right
        ast_op_equal,                    // left = right
        ast_op_not_equal,                // left != right
        ast_op_less,                    // left < right
        ast_op_greater,                    // left > right
        ast_op_less_or_equal,            // left <= right
        ast_op_greater_or_equal,        // left >= right
        ast_op_add,                        // left + right
        ast_op_subtract,                // left - right
        ast_op_multiply,                // left * right
        ast_op_divide,                    // left / right
        ast_op_mod,                        // left % right
        ast_op_negate,                    // left - right
        ast_op_union,                    // left | right
        ast_predicate,                    // apply predicate to set; next points to next predicate
        ast_filter,                        // select * from left where right
        ast_string_constant,            // string constant
        ast_number_constant,            // number constant
        ast_variable,                    // variable
        ast_func_last,                    // last()
        ast_func_position,                // position()
        ast_func_count,                    // count(left)
        ast_func_id,                    // id(left)
        ast_func_local_name_0,            // local-name()
        ast_func_local_name_1,            // local-name(left)
        ast_func_namespace_uri_0,        // namespace-uri()
        ast_func_namespace_uri_1,        // namespace-uri(left)
        ast_func_name_0,                // name()
        ast_func_name_1,                // name(left)
        ast_func_string_0,                // string()
        ast_func_string_1,                // string(left)
        ast_func_concat,                // concat(left, right, siblings)
        ast_func_starts_with,            // starts_with(left, right)
        ast_func_contains,                // contains(left, right)
        ast_func_substring_before,        // substring-before(left, right)
        ast_func_substring_after,        // substring-after(left, right)
        ast_func_substring_2,            // substring(left, right)
        ast_func_substring_3,            // substring(left, right, third)
        ast_func_string_length_0,        // string-length()
        ast_func_string_length_1,        // string-length(left)
        ast_func_normalize_space_0,        // normalize-space()
        ast_func_normalize_space_1,        // normalize-space(left)
        ast_func_translate,                // translate(left, right, third)
        ast_func_boolean,                // boolean(left)
        ast_func_not,                    // not(left)
        ast_func_true,                    // true()
        ast_func_false,                    // false()
        ast_func_lang,                    // lang(left)
        ast_func_number_0,                // number()
        ast_func_number_1,                // number(left)
        ast_func_sum,                    // sum(left)
        ast_func_floor,                    // floor(left)
        ast_func_ceiling,                // ceiling(left)
        ast_func_round,                    // round(left)
        ast_step,                        // process set left with step
        ast_step_root,                    // select root node

        ast_opt_translate_table,        // translate(left, right, third) where right/third are constants
        ast_opt_compare_attribute        // @name = 'string'
    };

    enum axis_t
    {
        axis_ancestor,
        axis_ancestor_or_self,
        axis_attribute,
        axis_child,
        axis_descendant,
        axis_descendant_or_self,
        axis_following,
        axis_following_sibling,
        axis_namespace,
        axis_parent,
        axis_preceding,
        axis_preceding_sibling,
        axis_self
    };
    
    enum nodetest_t
    {
        nodetest_none,
        nodetest_name,
        nodetest_type_node,
        nodetest_type_comment,
        nodetest_type_pi,
        nodetest_type_text,
        nodetest_pi,
        nodetest_all,
        nodetest_all_in_namespace
    };

    enum predicate_t
    {
        predicate_default,
        predicate_posinv,
        predicate_constant,
        predicate_constant_one
    };

    enum nodeset_eval_t
    {
        nodeset_eval_all,
        nodeset_eval_any,
        nodeset_eval_first
    };

    template <axis_t N> struct axis_to_type
    {
        static const axis_t axis;
    };

    template <axis_t N> const axis_t axis_to_type<N>::axis = N;
        
    class xpath_ast_node
    {
    private:
        // node type
        char _type;
        char _rettype;

        // for ast_step
        char _axis;

        // for ast_step/ast_predicate/ast_filter
        char _test;

        // tree node structure
        xpath_ast_node* _left;
        xpath_ast_node* _right;
        xpath_ast_node* _next;

        union
        {
            // value for ast_string_constant
            const char_t* string;
            // value for ast_number_constant
            double number;
            // variable for ast_variable
            xpath_variable* variable;
            // node test for ast_step (node name/namespace/node type/pi target)
            const char_t* nodetest;
            // table for ast_opt_translate_table
            const unsigned char* table;
        } _data;

        xpath_ast_node(const xpath_ast_node&);
        xpath_ast_node& operator=(const xpath_ast_node&);

        template <class Comp> static bool compare_eq(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const xpath_stack& stack, const Comp& comp)
        {
            xpath_value_type lt = lhs->rettype(), rt = rhs->rettype();

            if (lt != xpath_type_node_set && rt != xpath_type_node_set)
            {
                if (lt == xpath_type_boolean || rt == xpath_type_boolean)
                    return comp(lhs->eval_boolean(c, stack), rhs->eval_boolean(c, stack));
                else if (lt == xpath_type_number || rt == xpath_type_number)
                    return comp(lhs->eval_number(c, stack), rhs->eval_number(c, stack));
                else if (lt == xpath_type_string || rt == xpath_type_string)
                {
                    xpath_allocator_capture cr(stack.result);

                    xpath_string ls = lhs->eval_string(c, stack);
                    xpath_string rs = rhs->eval_string(c, stack);

                    return comp(ls, rs);
                }
            }
            else if (lt == xpath_type_node_set && rt == xpath_type_node_set)
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ls = lhs->eval_node_set(c, stack, nodeset_eval_all);
                xpath_node_set_raw rs = rhs->eval_node_set(c, stack, nodeset_eval_all);

                for (const xpath_node* li = ls.begin(); li != ls.end(); ++li)
                    for (const xpath_node* ri = rs.begin(); ri != rs.end(); ++ri)
                    {
                        xpath_allocator_capture cri(stack.result);

                        if (comp(string_value(*li, stack.result), string_value(*ri, stack.result)))
                            return true;
                    }

                return false;
            }
            else
            {
                if (lt == xpath_type_node_set)
                {
                    swap(lhs, rhs);
                    swap(lt, rt);
                }

                if (lt == xpath_type_boolean)
                    return comp(lhs->eval_boolean(c, stack), rhs->eval_boolean(c, stack));
                else if (lt == xpath_type_number)
                {
                    xpath_allocator_capture cr(stack.result);

                    double l = lhs->eval_number(c, stack);
                    xpath_node_set_raw rs = rhs->eval_node_set(c, stack, nodeset_eval_all);

                    for (const xpath_node* ri = rs.begin(); ri != rs.end(); ++ri)
                    {
                        xpath_allocator_capture cri(stack.result);

                        if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
                            return true;
                    }

                    return false;
                }
                else if (lt == xpath_type_string)
                {
                    xpath_allocator_capture cr(stack.result);

                    xpath_string l = lhs->eval_string(c, stack);
                    xpath_node_set_raw rs = rhs->eval_node_set(c, stack, nodeset_eval_all);

                    for (const xpath_node* ri = rs.begin(); ri != rs.end(); ++ri)
                    {
                        xpath_allocator_capture cri(stack.result);

                        if (comp(l, string_value(*ri, stack.result)))
                            return true;
                    }

                    return false;
                }
            }

            assert(!"Wrong types");
            return false;
        }

        static bool eval_once(xpath_node_set::type_t type, nodeset_eval_t eval)
        {
            return type == xpath_node_set::type_sorted ? eval != nodeset_eval_all : eval == nodeset_eval_any;
        }

        template <class Comp> static bool compare_rel(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const xpath_stack& stack, const Comp& comp)
        {
            xpath_value_type lt = lhs->rettype(), rt = rhs->rettype();

            if (lt != xpath_type_node_set && rt != xpath_type_node_set)
                return comp(lhs->eval_number(c, stack), rhs->eval_number(c, stack));
            else if (lt == xpath_type_node_set && rt == xpath_type_node_set)
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ls = lhs->eval_node_set(c, stack, nodeset_eval_all);
                xpath_node_set_raw rs = rhs->eval_node_set(c, stack, nodeset_eval_all);

                for (const xpath_node* li = ls.begin(); li != ls.end(); ++li)
                {
                    xpath_allocator_capture cri(stack.result);

                    double l = convert_string_to_number(string_value(*li, stack.result).c_str());

                    for (const xpath_node* ri = rs.begin(); ri != rs.end(); ++ri)
                    {
                        xpath_allocator_capture crii(stack.result);

                        if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
                            return true;
                    }
                }

                return false;
            }
            else if (lt != xpath_type_node_set && rt == xpath_type_node_set)
            {
                xpath_allocator_capture cr(stack.result);

                double l = lhs->eval_number(c, stack);
                xpath_node_set_raw rs = rhs->eval_node_set(c, stack, nodeset_eval_all);

                for (const xpath_node* ri = rs.begin(); ri != rs.end(); ++ri)
                {
                    xpath_allocator_capture cri(stack.result);

                    if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
                        return true;
                }

                return false;
            }
            else if (lt == xpath_type_node_set && rt != xpath_type_node_set)
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ls = lhs->eval_node_set(c, stack, nodeset_eval_all);
                double r = rhs->eval_number(c, stack);

                for (const xpath_node* li = ls.begin(); li != ls.end(); ++li)
                {
                    xpath_allocator_capture cri(stack.result);

                    if (comp(convert_string_to_number(string_value(*li, stack.result).c_str()), r))
                        return true;
                }

                return false;
            }
            else
            {
                assert(!"Wrong types");
                return false;
            }
        }

        static void apply_predicate_boolean(xpath_node_set_raw& ns, size_t first, xpath_ast_node* expr, const xpath_stack& stack, bool once)
        {
            assert(ns.size() >= first);
            assert(expr->rettype() != xpath_type_number);

            size_t i = 1;
            size_t size = ns.size() - first;

            xpath_node* last = ns.begin() + first;

            // remove_if... or well, sort of
            for (xpath_node* it = last; it != ns.end(); ++it, ++i)
            {
                xpath_context c(*it, i, size);

                if (expr->eval_boolean(c, stack))
                {
                    *last++ = *it;

                    if (once) break;
                }
            }

            ns.truncate(last);
        }

        static void apply_predicate_number(xpath_node_set_raw& ns, size_t first, xpath_ast_node* expr, const xpath_stack& stack, bool once)
        {
            assert(ns.size() >= first);
            assert(expr->rettype() == xpath_type_number);

            size_t i = 1;
            size_t size = ns.size() - first;

            xpath_node* last = ns.begin() + first;

            // remove_if... or well, sort of
            for (xpath_node* it = last; it != ns.end(); ++it, ++i)
            {
                xpath_context c(*it, i, size);

                if (expr->eval_number(c, stack) == i)
                {
                    *last++ = *it;

                    if (once) break;
                }
            }

            ns.truncate(last);
        }

        static void apply_predicate_number_const(xpath_node_set_raw& ns, size_t first, xpath_ast_node* expr, const xpath_stack& stack)
        {
            assert(ns.size() >= first);
            assert(expr->rettype() == xpath_type_number);

            size_t size = ns.size() - first;

            xpath_node* last = ns.begin() + first;

            xpath_context c(xpath_node(), 1, size);

            double er = expr->eval_number(c, stack);

            if (er >= 1.0 && er <= size)
            {
                size_t eri = static_cast<size_t>(er);

                if (er == eri)
                {
                    xpath_node r = last[eri - 1];

                    *last++ = r;
                }
            }

            ns.truncate(last);
        }

        void apply_predicate(xpath_node_set_raw& ns, size_t first, const xpath_stack& stack, bool once)
        {
            if (ns.size() == first) return;

            assert(_type == ast_filter || _type == ast_predicate);

            if (_test == predicate_constant || _test == predicate_constant_one)
                apply_predicate_number_const(ns, first, _right, stack);
            else if (_right->rettype() == xpath_type_number)
                apply_predicate_number(ns, first, _right, stack, once);
            else
                apply_predicate_boolean(ns, first, _right, stack, once);
        }

        void apply_predicates(xpath_node_set_raw& ns, size_t first, const xpath_stack& stack, nodeset_eval_t eval)
        {
            if (ns.size() == first) return;

            bool last_once = eval_once(ns.type(), eval);

            for (xpath_ast_node* pred = _right; pred; pred = pred->_next)
                pred->apply_predicate(ns, first, stack, !pred->_next && last_once);
        }

        bool step_push(xpath_node_set_raw& ns, xml_attribute_struct* a, xml_node_struct* parent, xpath_allocator* alloc)
        {
            assert(a);

            const char_t* name = a->name ? a->name : PUGIXML_TEXT("");

            switch (_test)
            {
            case nodetest_name:
                if (strequal(name, _data.nodetest) && is_xpath_attribute(name))
                {
                    ns.push_back(xpath_node(xml_attribute(a), xml_node(parent)), alloc);
                    return true;
                }
                break;
                
            case nodetest_type_node:
            case nodetest_all:
                if (is_xpath_attribute(name))
                {
                    ns.push_back(xpath_node(xml_attribute(a), xml_node(parent)), alloc);
                    return true;
                }
                break;
                
            case nodetest_all_in_namespace:
                if (starts_with(name, _data.nodetest) && is_xpath_attribute(name))
                {
                    ns.push_back(xpath_node(xml_attribute(a), xml_node(parent)), alloc);
                    return true;
                }
                break;
            
            default:
                ;
            }

            return false;
        }
        
        bool step_push(xpath_node_set_raw& ns, xml_node_struct* n, xpath_allocator* alloc)
        {
            assert(n);

            xml_node_type type = PUGI__NODETYPE(n);

            switch (_test)
            {
            case nodetest_name:
                if (type == node_element && n->name && strequal(n->name, _data.nodetest))
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                
            case nodetest_type_node:
                ns.push_back(xml_node(n), alloc);
                return true;
                
            case nodetest_type_comment:
                if (type == node_comment)
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                
            case nodetest_type_text:
                if (type == node_pcdata || type == node_cdata)
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                
            case nodetest_type_pi:
                if (type == node_pi)
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                                    
            case nodetest_pi:
                if (type == node_pi && n->name && strequal(n->name, _data.nodetest))
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                
            case nodetest_all:
                if (type == node_element)
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;
                
            case nodetest_all_in_namespace:
                if (type == node_element && n->name && starts_with(n->name, _data.nodetest))
                {
                    ns.push_back(xml_node(n), alloc);
                    return true;
                }
                break;

            default:
                assert(!"Unknown axis");
            }

            return false;
        }

        template <class T> void step_fill(xpath_node_set_raw& ns, xml_node_struct* n, xpath_allocator* alloc, bool once, T)
        {
            const axis_t axis = T::axis;

            switch (axis)
            {
            case axis_attribute:
            {
                for (xml_attribute_struct* a = n->first_attribute; a; a = a->next_attribute)
                    if (step_push(ns, a, n, alloc) & once)
                        return;
                
                break;
            }
            
            case axis_child:
            {
                for (xml_node_struct* c = n->first_child; c; c = c->next_sibling)
                    if (step_push(ns, c, alloc) & once)
                        return;
                    
                break;
            }
            
            case axis_descendant:
            case axis_descendant_or_self:
            {
                if (axis == axis_descendant_or_self)
                    if (step_push(ns, n, alloc) & once)
                        return;
                    
                xml_node_struct* cur = n->first_child;
                
                while (cur)
                {
                    if (step_push(ns, cur, alloc) & once)
                        return;
                    
                    if (cur->first_child)
                        cur = cur->first_child;
                    else
                    {
                        while (!cur->next_sibling)
                        {
                            cur = cur->parent;

                            if (cur == n) return;
                        }
                    
                        cur = cur->next_sibling;
                    }
                }
                
                break;
            }
            
            case axis_following_sibling:
            {
                for (xml_node_struct* c = n->next_sibling; c; c = c->next_sibling)
                    if (step_push(ns, c, alloc) & once)
                        return;
                
                break;
            }
            
            case axis_preceding_sibling:
            {
                for (xml_node_struct* c = n->prev_sibling_c; c->next_sibling; c = c->prev_sibling_c)
                    if (step_push(ns, c, alloc) & once)
                        return;
                
                break;
            }
            
            case axis_following:
            {
                xml_node_struct* cur = n;

                // exit from this node so that we don't include descendants
                while (!cur->next_sibling)
                {
                    cur = cur->parent;

                    if (!cur) return;
                }

                cur = cur->next_sibling;

                while (cur)
                {
                    if (step_push(ns, cur, alloc) & once)
                        return;

                    if (cur->first_child)
                        cur = cur->first_child;
                    else
                    {
                        while (!cur->next_sibling)
                        {
                            cur = cur->parent;

                            if (!cur) return;
                        }

                        cur = cur->next_sibling;
                    }
                }

                break;
            }

            case axis_preceding:
            {
                xml_node_struct* cur = n;

                // exit from this node so that we don't include descendants
                while (!cur->prev_sibling_c->next_sibling)
                {
                    cur = cur->parent;

                    if (!cur) return;
                }

                cur = cur->prev_sibling_c;

                while (cur)
                {
                    if (cur->first_child)
                        cur = cur->first_child->prev_sibling_c;
                    else
                    {
                        // leaf node, can't be ancestor
                        if (step_push(ns, cur, alloc) & once)
                            return;

                        while (!cur->prev_sibling_c->next_sibling)
                        {
                            cur = cur->parent;

                            if (!cur) return;

                            if (!node_is_ancestor(cur, n))
                                if (step_push(ns, cur, alloc) & once)
                                    return;
                        }

                        cur = cur->prev_sibling_c;
                    }
                }

                break;
            }
            
            case axis_ancestor:
            case axis_ancestor_or_self:
            {
                if (axis == axis_ancestor_or_self)
                    if (step_push(ns, n, alloc) & once)
                        return;

                xml_node_struct* cur = n->parent;
                
                while (cur)
                {
                    if (step_push(ns, cur, alloc) & once)
                        return;
                    
                    cur = cur->parent;
                }
                
                break;
            }

            case axis_self:
            {
                step_push(ns, n, alloc);

                break;
            }

            case axis_parent:
            {
                if (n->parent)
                    step_push(ns, n->parent, alloc);

                break;
            }
                
            default:
                assert(!"Unimplemented axis");
            }
        }
        
        template <class T> void step_fill(xpath_node_set_raw& ns, xml_attribute_struct* a, xml_node_struct* p, xpath_allocator* alloc, bool once, T v)
        {
            const axis_t axis = T::axis;

            switch (axis)
            {
            case axis_ancestor:
            case axis_ancestor_or_self:
            {
                if (axis == axis_ancestor_or_self && _test == nodetest_type_node) // reject attributes based on principal node type test
                    if (step_push(ns, a, p, alloc) & once)
                        return;

                xml_node_struct* cur = p;
                
                while (cur)
                {
                    if (step_push(ns, cur, alloc) & once)
                        return;
                    
                    cur = cur->parent;
                }
                
                break;
            }

            case axis_descendant_or_self:
            case axis_self:
            {
                if (_test == nodetest_type_node) // reject attributes based on principal node type test
                    step_push(ns, a, p, alloc);

                break;
            }

            case axis_following:
            {
                xml_node_struct* cur = p;
                
                while (cur)
                {
                    if (cur->first_child)
                        cur = cur->first_child;
                    else
                    {
                        while (!cur->next_sibling)
                        {
                            cur = cur->parent;

                            if (!cur) return;
                        }

                        cur = cur->next_sibling;
                    }

                    if (step_push(ns, cur, alloc) & once)
                        return;
                }

                break;
            }

            case axis_parent:
            {
                step_push(ns, p, alloc);

                break;
            }

            case axis_preceding:
            {
                // preceding:: axis does not include attribute nodes and attribute ancestors (they are the same as parent's ancestors), so we can reuse node preceding
                step_fill(ns, p, alloc, once, v);
                break;
            }
            
            default:
                assert(!"Unimplemented axis");
            }
        }

        template <class T> void step_fill(xpath_node_set_raw& ns, const xpath_node& xn, xpath_allocator* alloc, bool once, T v)
        {
            const axis_t axis = T::axis;
            const bool axis_has_attributes = (axis == axis_ancestor || axis == axis_ancestor_or_self || axis == axis_descendant_or_self || axis == axis_following || axis == axis_parent || axis == axis_preceding || axis == axis_self);

            if (xn.node())
                step_fill(ns, xn.node().internal_object(), alloc, once, v);
            else if (axis_has_attributes && xn.attribute() && xn.parent())
                step_fill(ns, xn.attribute().internal_object(), xn.parent().internal_object(), alloc, once, v);
        }

        template <class T> xpath_node_set_raw step_do(const xpath_context& c, const xpath_stack& stack, nodeset_eval_t eval, T v)
        {
            const axis_t axis = T::axis;
            const bool axis_reverse = (axis == axis_ancestor || axis == axis_ancestor_or_self || axis == axis_preceding || axis == axis_preceding_sibling);
            const xpath_node_set::type_t axis_type = axis_reverse ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_sorted;

            bool once =
                (axis == axis_attribute && _test == nodetest_name) ||
                (!_right && eval_once(axis_type, eval)) ||
                (_right && !_right->_next && _right->_test == predicate_constant_one);

            xpath_node_set_raw ns;
            ns.set_type(axis_type);

            if (_left)
            {
                xpath_node_set_raw s = _left->eval_node_set(c, stack, nodeset_eval_all);

                // self axis preserves the original order
                if (axis == axis_self) ns.set_type(s.type());

                for (const xpath_node* it = s.begin(); it != s.end(); ++it)
                {
                    size_t size = ns.size();

                    // in general, all axes generate elements in a particular order, but there is no order guarantee if axis is applied to two nodes
                    if (axis != axis_self && size != 0) ns.set_type(xpath_node_set::type_unsorted);
                    
                    step_fill(ns, *it, stack.result, once, v);
                    if (_right) apply_predicates(ns, size, stack, eval);
                }
            }
            else
            {
                step_fill(ns, c.n, stack.result, once, v);
                if (_right) apply_predicates(ns, 0, stack, eval);
            }

            // child, attribute and self axes always generate unique set of nodes
            // for other axis, if the set stayed sorted, it stayed unique because the traversal algorithms do not visit the same node twice
            if (axis != axis_child && axis != axis_attribute && axis != axis_self && ns.type() == xpath_node_set::type_unsorted)
                ns.remove_duplicates();

            return ns;
        }
        
    public:
        xpath_ast_node(ast_type_t type, xpath_value_type rettype_, const char_t* value):
            _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
        {
            assert(type == ast_string_constant);
            _data.string = value;
        }

        xpath_ast_node(ast_type_t type, xpath_value_type rettype_, double value):
            _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
        {
            assert(type == ast_number_constant);
            _data.number = value;
        }
        
        xpath_ast_node(ast_type_t type, xpath_value_type rettype_, xpath_variable* value):
            _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
        {
            assert(type == ast_variable);
            _data.variable = value;
        }
        
        xpath_ast_node(ast_type_t type, xpath_value_type rettype_, xpath_ast_node* left = 0, xpath_ast_node* right = 0):
            _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(left), _right(right), _next(0)
        {
        }

        xpath_ast_node(ast_type_t type, xpath_ast_node* left, axis_t axis, nodetest_t test, const char_t* contents):
            _type(static_cast<char>(type)), _rettype(xpath_type_node_set), _axis(static_cast<char>(axis)), _test(static_cast<char>(test)), _left(left), _right(0), _next(0)
        {
            assert(type == ast_step);
            _data.nodetest = contents;
        }

        xpath_ast_node(ast_type_t type, xpath_ast_node* left, xpath_ast_node* right, predicate_t test):
            _type(static_cast<char>(type)), _rettype(xpath_type_node_set), _axis(0), _test(static_cast<char>(test)), _left(left), _right(right), _next(0)
        {
            assert(type == ast_filter || type == ast_predicate);
        }

        void set_next(xpath_ast_node* value)
        {
            _next = value;
        }

        void set_right(xpath_ast_node* value)
        {
            _right = value;
        }

        bool eval_boolean(const xpath_context& c, const xpath_stack& stack)
        {
            switch (_type)
            {
            case ast_op_or:
                return _left->eval_boolean(c, stack) || _right->eval_boolean(c, stack);
                
            case ast_op_and:
                return _left->eval_boolean(c, stack) && _right->eval_boolean(c, stack);
                
            case ast_op_equal:
                return compare_eq(_left, _right, c, stack, equal_to());

            case ast_op_not_equal:
                return compare_eq(_left, _right, c, stack, not_equal_to());
    
            case ast_op_less:
                return compare_rel(_left, _right, c, stack, less());
            
            case ast_op_greater:
                return compare_rel(_right, _left, c, stack, less());

            case ast_op_less_or_equal:
                return compare_rel(_left, _right, c, stack, less_equal());
            
            case ast_op_greater_or_equal:
                return compare_rel(_right, _left, c, stack, less_equal());

            case ast_func_starts_with:
            {
                xpath_allocator_capture cr(stack.result);

                xpath_string lr = _left->eval_string(c, stack);
                xpath_string rr = _right->eval_string(c, stack);

                return starts_with(lr.c_str(), rr.c_str());
            }

            case ast_func_contains:
            {
                xpath_allocator_capture cr(stack.result);

                xpath_string lr = _left->eval_string(c, stack);
                xpath_string rr = _right->eval_string(c, stack);

                return find_substring(lr.c_str(), rr.c_str()) != 0;
            }

            case ast_func_boolean:
                return _left->eval_boolean(c, stack);
                
            case ast_func_not:
                return !_left->eval_boolean(c, stack);
                
            case ast_func_true:
                return true;
                
            case ast_func_false:
                return false;

            case ast_func_lang:
            {
                if (c.n.attribute()) return false;
                
                xpath_allocator_capture cr(stack.result);

                xpath_string lang = _left->eval_string(c, stack);
                
                for (xml_node n = c.n.node(); n; n = n.parent())
                {
                    xml_attribute a = n.attribute(PUGIXML_TEXT("xml:lang"));
                    
                    if (a)
                    {
                        const char_t* value = a.value();
                        
                        // strnicmp / strncasecmp is not portable
                        for (const char_t* lit = lang.c_str(); *lit; ++lit)
                        {
                            if (tolower_ascii(*lit) != tolower_ascii(*value)) return false;
                            ++value;
                        }
                        
                        return *value == 0 || *value == '-';
                    }
                }
                
                return false;
            }

            case ast_opt_compare_attribute:
            {
                const char_t* value = (_right->_type == ast_string_constant) ? _right->_data.string : _right->_data.variable->get_string();

                xml_attribute attr = c.n.node().attribute(_left->_data.nodetest);

                return attr && strequal(attr.value(), value) && is_xpath_attribute(attr.name());
            }

            case ast_variable:
            {
                assert(_rettype == _data.variable->type());

                if (_rettype == xpath_type_boolean)
                    return _data.variable->get_boolean();

                // fallthrough to type conversion
            }

            default:
            {
                switch (_rettype)
                {
                case xpath_type_number:
                    return convert_number_to_boolean(eval_number(c, stack));
                    
                case xpath_type_string:
                {
                    xpath_allocator_capture cr(stack.result);

                    return !eval_string(c, stack).empty();
                }
                    
                case xpath_type_node_set:                
                {
                    xpath_allocator_capture cr(stack.result);

                    return !eval_node_set(c, stack, nodeset_eval_any).empty();
                }

                default:
                    assert(!"Wrong expression for return type boolean");
                    return false;
                }
            }
            }
        }

        double eval_number(const xpath_context& c, const xpath_stack& stack)
        {
            switch (_type)
            {
            case ast_op_add:
                return _left->eval_number(c, stack) + _right->eval_number(c, stack);
                
            case ast_op_subtract:
                return _left->eval_number(c, stack) - _right->eval_number(c, stack);

            case ast_op_multiply:
                return _left->eval_number(c, stack) * _right->eval_number(c, stack);

            case ast_op_divide:
                return _left->eval_number(c, stack) / _right->eval_number(c, stack);

            case ast_op_mod:
                return fmod(_left->eval_number(c, stack), _right->eval_number(c, stack));

            case ast_op_negate:
                return -_left->eval_number(c, stack);

            case ast_number_constant:
                return _data.number;

            case ast_func_last:
                return static_cast<double>(c.size);
            
            case ast_func_position:
                return static_cast<double>(c.position);

            case ast_func_count:
            {
                xpath_allocator_capture cr(stack.result);

                return static_cast<double>(_left->eval_node_set(c, stack, nodeset_eval_all).size());
            }
            
            case ast_func_string_length_0:
            {
                xpath_allocator_capture cr(stack.result);

                return static_cast<double>(string_value(c.n, stack.result).length());
            }
            
            case ast_func_string_length_1:
            {
                xpath_allocator_capture cr(stack.result);

                return static_cast<double>(_left->eval_string(c, stack).length());
            }
            
            case ast_func_number_0:
            {
                xpath_allocator_capture cr(stack.result);

                return convert_string_to_number(string_value(c.n, stack.result).c_str());
            }
            
            case ast_func_number_1:
                return _left->eval_number(c, stack);

            case ast_func_sum:
            {
                xpath_allocator_capture cr(stack.result);

                double r = 0;
                
                xpath_node_set_raw ns = _left->eval_node_set(c, stack, nodeset_eval_all);
                
                for (const xpath_node* it = ns.begin(); it != ns.end(); ++it)
                {
                    xpath_allocator_capture cri(stack.result);

                    r += convert_string_to_number(string_value(*it, stack.result).c_str());
                }
            
                return r;
            }

            case ast_func_floor:
            {
                double r = _left->eval_number(c, stack);
                
                return r == r ? floor(r) : r;
            }

            case ast_func_ceiling:
            {
                double r = _left->eval_number(c, stack);
                
                return r == r ? ceil(r) : r;
            }

            case ast_func_round:
                return round_nearest_nzero(_left->eval_number(c, stack));
            
            case ast_variable:
            {
                assert(_rettype == _data.variable->type());

                if (_rettype == xpath_type_number)
                    return _data.variable->get_number();

                // fallthrough to type conversion
            }

            default:
            {
                switch (_rettype)
                {
                case xpath_type_boolean:
                    return eval_boolean(c, stack) ? 1 : 0;
                    
                case xpath_type_string:
                {
                    xpath_allocator_capture cr(stack.result);

                    return convert_string_to_number(eval_string(c, stack).c_str());
                }
                    
                case xpath_type_node_set:
                {
                    xpath_allocator_capture cr(stack.result);

                    return convert_string_to_number(eval_string(c, stack).c_str());
                }
                    
                default:
                    assert(!"Wrong expression for return type number");
                    return 0;
                }
                
            }
            }
        }
        
        xpath_string eval_string_concat(const xpath_context& c, const xpath_stack& stack)
        {
            assert(_type == ast_func_concat);

            xpath_allocator_capture ct(stack.temp);

            // count the string number
            size_t count = 1;
            for (xpath_ast_node* nc = _right; nc; nc = nc->_next) count++;

            // gather all strings
            xpath_string static_buffer[4];
            xpath_string* buffer = static_buffer;

            // allocate on-heap for large concats
            if (count > sizeof(static_buffer) / sizeof(static_buffer[0]))
            {
                buffer = static_cast<xpath_string*>(stack.temp->allocate(count * sizeof(xpath_string)));
                assert(buffer);
            }

            // evaluate all strings to temporary stack
            xpath_stack swapped_stack = {stack.temp, stack.result};

            buffer[0] = _left->eval_string(c, swapped_stack);

            size_t pos = 1;
            for (xpath_ast_node* n = _right; n; n = n->_next, ++pos) buffer[pos] = n->eval_string(c, swapped_stack);
            assert(pos == count);

            // get total length
            size_t length = 0;
            for (size_t i = 0; i < count; ++i) length += buffer[i].length();

            // create final string
            char_t* result = static_cast<char_t*>(stack.result->allocate((length + 1) * sizeof(char_t)));
            assert(result);

            char_t* ri = result;

            for (size_t j = 0; j < count; ++j)
                for (const char_t* bi = buffer[j].c_str(); *bi; ++bi)
                    *ri++ = *bi;

            *ri = 0;

            return xpath_string::from_heap_preallocated(result, ri);
        }

        xpath_string eval_string(const xpath_context& c, const xpath_stack& stack)
        {
            switch (_type)
            {
            case ast_string_constant:
                return xpath_string::from_const(_data.string);
            
            case ast_func_local_name_0:
            {
                xpath_node na = c.n;
                
                return xpath_string::from_const(local_name(na));
            }

            case ast_func_local_name_1:
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ns = _left->eval_node_set(c, stack, nodeset_eval_first);
                xpath_node na = ns.first();
                
                return xpath_string::from_const(local_name(na));
            }

            case ast_func_name_0:
            {
                xpath_node na = c.n;
                
                return xpath_string::from_const(qualified_name(na));
            }

            case ast_func_name_1:
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ns = _left->eval_node_set(c, stack, nodeset_eval_first);
                xpath_node na = ns.first();
                
                return xpath_string::from_const(qualified_name(na));
            }

            case ast_func_namespace_uri_0:
            {
                xpath_node na = c.n;
                
                return xpath_string::from_const(namespace_uri(na));
            }

            case ast_func_namespace_uri_1:
            {
                xpath_allocator_capture cr(stack.result);

                xpath_node_set_raw ns = _left->eval_node_set(c, stack, nodeset_eval_first);
                xpath_node na = ns.first();
                
                return xpath_string::from_const(namespace_uri(na));
            }

            case ast_func_string_0:
                return string_value(c.n, stack.result);

            case ast_func_string_1:
                return _left->eval_string(c, stack);

            case ast_func_concat:
                return eval_string_concat(c, stack);

            case ast_func_substring_before:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_string s = _left->eval_string(c, swapped_stack);
                xpath_string p = _right->eval_string(c, swapped_stack);

                const char_t* pos = find_substring(s.c_str(), p.c_str());
                
                return pos ? xpath_string::from_heap(s.c_str(), pos, stack.result) : xpath_string();
            }
            
            case ast_func_substring_after:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_string s = _left->eval_string(c, swapped_stack);
                xpath_string p = _right->eval_string(c, swapped_stack);
                
                const char_t* pos = find_substring(s.c_str(), p.c_str());
                if (!pos) return xpath_string();

                const char_t* rbegin = pos + p.length();
                const char_t* rend = s.c_str() + s.length();

                return s.uses_heap() ? xpath_string::from_heap(rbegin, rend, stack.result) : xpath_string::from_const(rbegin);
            }

            case ast_func_substring_2:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_string s = _left->eval_string(c, swapped_stack);
                size_t s_length = s.length();

                double first = round_nearest(_right->eval_number(c, stack));
                
                if (is_nan(first)) return xpath_string(); // NaN
                else if (first >= s_length + 1) return xpath_string();
                
                size_t pos = first < 1 ? 1 : static_cast<size_t>(first);
                assert(1 <= pos && pos <= s_length + 1);

                const char_t* rbegin = s.c_str() + (pos - 1);
                const char_t* rend = s.c_str() + s.length();
                
                return s.uses_heap() ? xpath_string::from_heap(rbegin, rend, stack.result) : xpath_string::from_const(rbegin);
            }
            
            case ast_func_substring_3:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_string s = _left->eval_string(c, swapped_stack);
                size_t s_length = s.length();

                double first = round_nearest(_right->eval_number(c, stack));
                double last = first + round_nearest(_right->_next->eval_number(c, stack));
                
                if (is_nan(first) || is_nan(last)) return xpath_string();
                else if (first >= s_length + 1) return xpath_string();
                else if (first >= last) return xpath_string();
                else if (last < 1) return xpath_string();
                
                size_t pos = first < 1 ? 1 : static_cast<size_t>(first);
                size_t end = last >= s_length + 1 ? s_length + 1 : static_cast<size_t>(last);

                assert(1 <= pos && pos <= end && end <= s_length + 1);
                const char_t* rbegin = s.c_str() + (pos - 1);
                const char_t* rend = s.c_str() + (end - 1);

                return (end == s_length + 1 && !s.uses_heap()) ? xpath_string::from_const(rbegin) : xpath_string::from_heap(rbegin, rend, stack.result);
            }

            case ast_func_normalize_space_0:
            {
                xpath_string s = string_value(c.n, stack.result);

                char_t* begin = s.data(stack.result);
                char_t* end = normalize_space(begin);

                return xpath_string::from_heap_preallocated(begin, end);
            }

            case ast_func_normalize_space_1:
            {
                xpath_string s = _left->eval_string(c, stack);

                char_t* begin = s.data(stack.result);
                char_t* end = normalize_space(begin);
            
                return xpath_string::from_heap_preallocated(begin, end);
            }

            case ast_func_translate:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_string s = _left->eval_string(c, stack);
                xpath_string from = _right->eval_string(c, swapped_stack);
                xpath_string to = _right->_next->eval_string(c, swapped_stack);

                char_t* begin = s.data(stack.result);
                char_t* end = translate(begin, from.c_str(), to.c_str(), to.length());

                return xpath_string::from_heap_preallocated(begin, end);
            }

            case ast_opt_translate_table:
            {
                xpath_string s = _left->eval_string(c, stack);

                char_t* begin = s.data(stack.result);
                char_t* end = translate_table(begin, _data.table);

                return xpath_string::from_heap_preallocated(begin, end);
            }

            case ast_variable:
            {
                assert(_rettype == _data.variable->type());

                if (_rettype == xpath_type_string)
                    return xpath_string::from_const(_data.variable->get_string());

                // fallthrough to type conversion
            }

            default:
            {
                switch (_rettype)
                {
                case xpath_type_boolean:
                    return xpath_string::from_const(eval_boolean(c, stack) ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false"));
                    
                case xpath_type_number:
                    return convert_number_to_string(eval_number(c, stack), stack.result);
                    
                case xpath_type_node_set:
                {
                    xpath_allocator_capture cr(stack.temp);

                    xpath_stack swapped_stack = {stack.temp, stack.result};

                    xpath_node_set_raw ns = eval_node_set(c, swapped_stack, nodeset_eval_first);
                    return ns.empty() ? xpath_string() : string_value(ns.first(), stack.result);
                }
                
                default:
                    assert(!"Wrong expression for return type string");
                    return xpath_string();
                }
            }
            }
        }

        xpath_node_set_raw eval_node_set(const xpath_context& c, const xpath_stack& stack, nodeset_eval_t eval)
        {
            switch (_type)
            {
            case ast_op_union:
            {
                xpath_allocator_capture cr(stack.temp);

                xpath_stack swapped_stack = {stack.temp, stack.result};

                xpath_node_set_raw ls = _left->eval_node_set(c, swapped_stack, eval);
                xpath_node_set_raw rs = _right->eval_node_set(c, stack, eval);

                // we can optimize merging two sorted sets, but this is a very rare operation, so don't bother
                rs.set_type(xpath_node_set::type_unsorted);

                rs.append(ls.begin(), ls.end(), stack.result);
                rs.remove_duplicates();

                return rs;
            }

            case ast_filter:
            {
                xpath_node_set_raw set = _left->eval_node_set(c, stack, _test == predicate_constant_one ? nodeset_eval_first : nodeset_eval_all);

                // either expression is a number or it contains position() call; sort by document order
                if (_test != predicate_posinv) set.sort_do();

                bool once = eval_once(set.type(), eval);

                apply_predicate(set, 0, stack, once);
            
                return set;
            }
            
            case ast_func_id:
                return xpath_node_set_raw();
            
            case ast_step:
            {
                switch (_axis)
                {
                case axis_ancestor:
                    return step_do(c, stack, eval, axis_to_type<axis_ancestor>());
                    
                case axis_ancestor_or_self:
                    return step_do(c, stack, eval, axis_to_type<axis_ancestor_or_self>());

                case axis_attribute:
                    return step_do(c, stack, eval, axis_to_type<axis_attribute>());

                case axis_child:
                    return step_do(c, stack, eval, axis_to_type<axis_child>());
                
                case axis_descendant:
                    return step_do(c, stack, eval, axis_to_type<axis_descendant>());

                case axis_descendant_or_self:
                    return step_do(c, stack, eval, axis_to_type<axis_descendant_or_self>());

                case axis_following:
                    return step_do(c, stack, eval, axis_to_type<axis_following>());
                
                case axis_following_sibling:
                    return step_do(c, stack, eval, axis_to_type<axis_following_sibling>());
                
                case axis_namespace:
                    // namespaced axis is not supported
                    return xpath_node_set_raw();
                
                case axis_parent:
                    return step_do(c, stack, eval, axis_to_type<axis_parent>());
                
                case axis_preceding:
                    return step_do(c, stack, eval, axis_to_type<axis_preceding>());

                case axis_preceding_sibling:
                    return step_do(c, stack, eval, axis_to_type<axis_preceding_sibling>());
                
                case axis_self:
                    return step_do(c, stack, eval, axis_to_type<axis_self>());

                default:
                    assert(!"Unknown axis");
                    return xpath_node_set_raw();
                }
            }

            case ast_step_root:
            {
                assert(!_right); // root step can't have any predicates

                xpath_node_set_raw ns;

                ns.set_type(xpath_node_set::type_sorted);

                if (c.n.node()) ns.push_back(c.n.node().root(), stack.result);
                else if (c.n.attribute()) ns.push_back(c.n.parent().root(), stack.result);

                return ns;
            }

            case ast_variable:
            {
                assert(_rettype == _data.variable->type());

                if (_rettype == xpath_type_node_set)
                {
                    const xpath_node_set& s = _data.variable->get_node_set();

                    xpath_node_set_raw ns;

                    ns.set_type(s.type());
                    ns.append(s.begin(), s.end(), stack.result);

                    return ns;
                }

                // fallthrough to type conversion
            }

            default:
                assert(!"Wrong expression for return type node set");
                return xpath_node_set_raw();
            }
        }

        void optimize(xpath_allocator* alloc)
        {
            if (_left) _left->optimize(alloc);
            if (_right) _right->optimize(alloc);
            if (_next) _next->optimize(alloc);

            // Rewrite [position()=expr] with [expr]
            // Note that this step has to go before classification to recognize [position()=1]
            if ((_type == ast_filter || _type == ast_predicate) &&
                _right->_type == ast_op_equal && _right->_left->_type == ast_func_position && _right->_right->_rettype == xpath_type_number)
            {
                _right = _right->_right;
            }

            // Classify filter/predicate ops to perform various optimizations during evaluation
            if (_type == ast_filter || _type == ast_predicate)
            {
                assert(_test == predicate_default);

                if (_right->_type == ast_number_constant && _right->_data.number == 1.0)
                    _test = predicate_constant_one;
                else if (_right->_rettype == xpath_type_number && (_right->_type == ast_number_constant || _right->_type == ast_variable || _right->_type == ast_func_last))
                    _test = predicate_constant;
                else if (_right->_rettype != xpath_type_number && _right->is_posinv_expr())
                    _test = predicate_posinv;
            }

            // Rewrite descendant-or-self::node()/child::foo with descendant::foo
            // The former is a full form of //foo, the latter is much faster since it executes the node test immediately
            // Do a similar kind of rewrite for self/descendant/descendant-or-self axes
            // Note that we only rewrite positionally invariant steps (//foo[1] != /descendant::foo[1])
            if (_type == ast_step && (_axis == axis_child || _axis == axis_self || _axis == axis_descendant || _axis == axis_descendant_or_self) && _left &&
                _left->_type == ast_step && _left->_axis == axis_descendant_or_self && _left->_test == nodetest_type_node && !_left->_right &&
                is_posinv_step())
            {
                if (_axis == axis_child || _axis == axis_descendant)
                    _axis = axis_descendant;
                else
                    _axis = axis_descendant_or_self;

                _left = _left->_left;
            }

            // Use optimized lookup table implementation for translate() with constant arguments
            if (_type == ast_func_translate && _right->_type == ast_string_constant && _right->_next->_type == ast_string_constant)
            {
                unsigned char* table = translate_table_generate(alloc, _right->_data.string, _right->_next->_data.string);

                if (table)
                {
                    _type = ast_opt_translate_table;
                    _data.table = table;
                }
            }

            // Use optimized path for @attr = 'value' or @attr = $value
            if (_type == ast_op_equal &&
                _left->_type == ast_step && _left->_axis == axis_attribute && _left->_test == nodetest_name && !_left->_left && !_left->_right &&
                (_right->_type == ast_string_constant || (_right->_type == ast_variable && _right->_rettype == xpath_type_string)))
            {
                _type = ast_opt_compare_attribute;
            }
        }
        
        bool is_posinv_expr() const
        {
            switch (_type)
            {
            case ast_func_position:
            case ast_func_last:
                return false;

            case ast_string_constant:
            case ast_number_constant:
            case ast_variable:
                return true;

            case ast_step:
            case ast_step_root:
                return true;

            case ast_predicate:
            case ast_filter:
                return true;

            default:
                if (_left && !_left->is_posinv_expr()) return false;
                
                for (xpath_ast_node* n = _right; n; n = n->_next)
                    if (!n->is_posinv_expr()) return false;
                    
                return true;
            }
        }

        bool is_posinv_step() const
        {
            assert(_type == ast_step);

            for (xpath_ast_node* n = _right; n; n = n->_next)
            {
                assert(n->_type == ast_predicate);

                if (n->_test != predicate_posinv)
                    return false;
            }

            return true;
        }

        xpath_value_type rettype() const
        {
            return static_cast<xpath_value_type>(_rettype);
        }
    };

    struct xpath_parser
    {
        xpath_allocator* _alloc;
        xpath_lexer _lexer;

        const char_t* _query;
        xpath_variable_set* _variables;

        xpath_parse_result* _result;

        char_t _scratch[32];

    #ifdef PUGIXML_NO_EXCEPTIONS
        jmp_buf _error_handler;
    #endif

        void throw_error(const char* message)
        {
            _result->error = message;
            _result->offset = _lexer.current_pos() - _query;

        #ifdef PUGIXML_NO_EXCEPTIONS
            longjmp(_error_handler, 1);
        #else
            throw xpath_exception(*_result);
        #endif
        }

        void throw_error_oom()
        {
        #ifdef PUGIXML_NO_EXCEPTIONS
            throw_error("Out of memory");
        #else
            throw std::bad_alloc();
        #endif
        }

        void* alloc_node()
        {
            void* result = _alloc->allocate_nothrow(sizeof(xpath_ast_node));

            if (!result) throw_error_oom();

            return result;
        }

        const char_t* alloc_string(const xpath_lexer_string& value)
        {
            if (value.begin)
            {
                size_t length = static_cast<size_t>(value.end - value.begin);

                char_t* c = static_cast<char_t*>(_alloc->allocate_nothrow((length + 1) * sizeof(char_t)));
                if (!c) throw_error_oom();
                assert(c); // workaround for clang static analysis

                memcpy(c, value.begin, length * sizeof(char_t));
                c[length] = 0;

                return c;
            }
            else return 0;
        }

        xpath_ast_node* parse_function_helper(ast_type_t type0, ast_type_t type1, size_t argc, xpath_ast_node* args[2])
        {
            assert(argc <= 1);

            if (argc == 1 && args[0]->rettype() != xpath_type_node_set) throw_error("Function has to be applied to node set");

            return new (alloc_node()) xpath_ast_node(argc == 0 ? type0 : type1, xpath_type_string, args[0]);
        }

        xpath_ast_node* parse_function(const xpath_lexer_string& name, size_t argc, xpath_ast_node* args[2])
        {
            switch (name.begin[0])
            {
            case 'b':
                if (name == PUGIXML_TEXT("boolean") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_boolean, xpath_type_boolean, args[0]);
                    
                break;
            
            case 'c':
                if (name == PUGIXML_TEXT("count") && argc == 1)
                {
                    if (args[0]->rettype() != xpath_type_node_set) throw_error("Function has to be applied to node set");
                    return new (alloc_node()) xpath_ast_node(ast_func_count, xpath_type_number, args[0]);
                }
                else if (name == PUGIXML_TEXT("contains") && argc == 2)
                    return new (alloc_node()) xpath_ast_node(ast_func_contains, xpath_type_boolean, args[0], args[1]);
                else if (name == PUGIXML_TEXT("concat") && argc >= 2)
                    return new (alloc_node()) xpath_ast_node(ast_func_concat, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("ceiling") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_ceiling, xpath_type_number, args[0]);
                    
                break;
            
            case 'f':
                if (name == PUGIXML_TEXT("false") && argc == 0)
                    return new (alloc_node()) xpath_ast_node(ast_func_false, xpath_type_boolean);
                else if (name == PUGIXML_TEXT("floor") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_floor, xpath_type_number, args[0]);
                    
                break;
            
            case 'i':
                if (name == PUGIXML_TEXT("id") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_id, xpath_type_node_set, args[0]);
                    
                break;
            
            case 'l':
                if (name == PUGIXML_TEXT("last") && argc == 0)
                    return new (alloc_node()) xpath_ast_node(ast_func_last, xpath_type_number);
                else if (name == PUGIXML_TEXT("lang") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_lang, xpath_type_boolean, args[0]);
                else if (name == PUGIXML_TEXT("local-name") && argc <= 1)
                    return parse_function_helper(ast_func_local_name_0, ast_func_local_name_1, argc, args);
            
                break;
            
            case 'n':
                if (name == PUGIXML_TEXT("name") && argc <= 1)
                    return parse_function_helper(ast_func_name_0, ast_func_name_1, argc, args);
                else if (name == PUGIXML_TEXT("namespace-uri") && argc <= 1)
                    return parse_function_helper(ast_func_namespace_uri_0, ast_func_namespace_uri_1, argc, args);
                else if (name == PUGIXML_TEXT("normalize-space") && argc <= 1)
                    return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_normalize_space_0 : ast_func_normalize_space_1, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("not") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_not, xpath_type_boolean, args[0]);
                else if (name == PUGIXML_TEXT("number") && argc <= 1)
                    return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_number_0 : ast_func_number_1, xpath_type_number, args[0]);
            
                break;
            
            case 'p':
                if (name == PUGIXML_TEXT("position") && argc == 0)
                    return new (alloc_node()) xpath_ast_node(ast_func_position, xpath_type_number);
                
                break;
            
            case 'r':
                if (name == PUGIXML_TEXT("round") && argc == 1)
                    return new (alloc_node()) xpath_ast_node(ast_func_round, xpath_type_number, args[0]);

                break;
            
            case 's':
                if (name == PUGIXML_TEXT("string") && argc <= 1)
                    return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_0 : ast_func_string_1, xpath_type_string, args[0]);
                else if (name == PUGIXML_TEXT("string-length") && argc <= 1)
                    return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_length_0 : ast_func_string_length_1, xpath_type_number, args[0]);
                else if (name == PUGIXML_TEXT("starts-with") && argc == 2)
                    return new (alloc_node()) xpath_ast_node(ast_func_starts_with, xpath_type_boolean, args[0], args[1]);
                else if (name == PUGIXML_TEXT("substring-before") && argc == 2)
                    return new (alloc_node()) xpath_ast_node(ast_func_substring_before, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("substring-after") && argc == 2)
                    return new (alloc_node()) xpath_ast_node(ast_func_substring_after, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("substring") && (argc == 2 || argc == 3))
                    return new (alloc_node()) xpath_ast_node(argc == 2 ? ast_func_substring_2 : ast_func_substring_3, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("sum") && argc == 1)
                {
                    if (args[0]->rettype() != xpath_type_node_set) throw_error("Function has to be applied to node set");
                    return new (alloc_node()) xpath_ast_node(ast_func_sum, xpath_type_number, args[0]);
                }

                break;
            
            case 't':
                if (name == PUGIXML_TEXT("translate") && argc == 3)
                    return new (alloc_node()) xpath_ast_node(ast_func_translate, xpath_type_string, args[0], args[1]);
                else if (name == PUGIXML_TEXT("true") && argc == 0)
                    return new (alloc_node()) xpath_ast_node(ast_func_true, xpath_type_boolean);
                    
                break;

            default:
                break;
            }

            throw_error("Unrecognized function or wrong parameter count");

            return 0;
        }

        axis_t parse_axis_name(const xpath_lexer_string& name, bool& specified)
        {
            specified = true;

            switch (name.begin[0])
            {
            case 'a':
                if (name == PUGIXML_TEXT("ancestor"))
                    return axis_ancestor;
                else if (name == PUGIXML_TEXT("ancestor-or-self"))
                    return axis_ancestor_or_self;
                else if (name == PUGIXML_TEXT("attribute"))
                    return axis_attribute;
                
                break;
            
            case 'c':
                if (name == PUGIXML_TEXT("child"))
                    return axis_child;
                
                break;
            
            case 'd':
                if (name == PUGIXML_TEXT("descendant"))
                    return axis_descendant;
                else if (name == PUGIXML_TEXT("descendant-or-self"))
                    return axis_descendant_or_self;
                
                break;
            
            case 'f':
                if (name == PUGIXML_TEXT("following"))
                    return axis_following;
                else if (name == PUGIXML_TEXT("following-sibling"))
                    return axis_following_sibling;
                
                break;
            
            case 'n':
                if (name == PUGIXML_TEXT("namespace"))
                    return axis_namespace;
                
                break;
            
            case 'p':
                if (name == PUGIXML_TEXT("parent"))
                    return axis_parent;
                else if (name == PUGIXML_TEXT("preceding"))
                    return axis_preceding;
                else if (name == PUGIXML_TEXT("preceding-sibling"))
                    return axis_preceding_sibling;
                
                break;
            
            case 's':
                if (name == PUGIXML_TEXT("self"))
                    return axis_self;
                
                break;

            default:
                break;
            }

            specified = false;
            return axis_child;
        }

        nodetest_t parse_node_test_type(const xpath_lexer_string& name)
        {
            switch (name.begin[0])
            {
            case 'c':
                if (name == PUGIXML_TEXT("comment"))
                    return nodetest_type_comment;

                break;

            case 'n':
                if (name == PUGIXML_TEXT("node"))
                    return nodetest_type_node;

                break;

            case 'p':
                if (name == PUGIXML_TEXT("processing-instruction"))
                    return nodetest_type_pi;

                break;

            case 't':
                if (name == PUGIXML_TEXT("text"))
                    return nodetest_type_text;

                break;
            
            default:
                break;
            }

            return nodetest_none;
        }

        // PrimaryExpr ::= VariableReference | '(' Expr ')' | Literal | Number | FunctionCall
        xpath_ast_node* parse_primary_expression()
        {
            switch (_lexer.current())
            {
            case lex_var_ref:
            {
                xpath_lexer_string name = _lexer.contents();

                if (!_variables)
                    throw_error("Unknown variable: variable set is not provided");

                xpath_variable* var = get_variable_scratch(_scratch, _variables, name.begin, name.end);

                if (!var)
                    throw_error("Unknown variable: variable set does not contain the given name");

                _lexer.next();

                return new (alloc_node()) xpath_ast_node(ast_variable, var->type(), var);
            }

            case lex_open_brace:
            {
                _lexer.next();

                xpath_ast_node* n = parse_expression();

                if (_lexer.current() != lex_close_brace)
                    throw_error("Unmatched braces");

                _lexer.next();

                return n;
            }

            case lex_quoted_string:
            {
                const char_t* value = alloc_string(_lexer.contents());

                xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_string_constant, xpath_type_string, value);
                _lexer.next();

                return n;
            }

            case lex_number:
            {
                double value = 0;

                if (!convert_string_to_number_scratch(_scratch, _lexer.contents().begin, _lexer.contents().end, &value))
                    throw_error_oom();

                xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_number_constant, xpath_type_number, value);
                _lexer.next();

                return n;
            }

            case lex_string:
            {
                xpath_ast_node* args[2] = {0};
                size_t argc = 0;
                
                xpath_lexer_string function = _lexer.contents();
                _lexer.next();
                
                xpath_ast_node* last_arg = 0;
                
                if (_lexer.current() != lex_open_brace)
                    throw_error("Unrecognized function call");
                _lexer.next();

                if (_lexer.current() != lex_close_brace)
                    args[argc++] = parse_expression();

                while (_lexer.current() != lex_close_brace)
                {
                    if (_lexer.current() != lex_comma)
                        throw_error("No comma between function arguments");
                    _lexer.next();
                    
                    xpath_ast_node* n = parse_expression();
                    
                    if (argc < 2) args[argc] = n;
                    else last_arg->set_next(n);

                    argc++;
                    last_arg = n;
                }
                
                _lexer.next();

                return parse_function(function, argc, args);
            }

            default:
                throw_error("Unrecognizable primary expression");

                return 0;
            }
        }
        
        // FilterExpr ::= PrimaryExpr | FilterExpr Predicate
        // Predicate ::= '[' PredicateExpr ']'
        // PredicateExpr ::= Expr
        xpath_ast_node* parse_filter_expression()
        {
            xpath_ast_node* n = parse_primary_expression();

            while (_lexer.current() == lex_open_square_brace)
            {
                _lexer.next();

                xpath_ast_node* expr = parse_expression();

                if (n->rettype() != xpath_type_node_set) throw_error("Predicate has to be applied to node set");

                n = new (alloc_node()) xpath_ast_node(ast_filter, n, expr, predicate_default);

                if (_lexer.current() != lex_close_square_brace)
                    throw_error("Unmatched square brace");
            
                _lexer.next();
            }
            
            return n;
        }
        
        // Step ::= AxisSpecifier NodeTest Predicate* | AbbreviatedStep
        // AxisSpecifier ::= AxisName '::' | '@'?
        // NodeTest ::= NameTest | NodeType '(' ')' | 'processing-instruction' '(' Literal ')'
        // NameTest ::= '*' | NCName ':' '*' | QName
        // AbbreviatedStep ::= '.' | '..'
        xpath_ast_node* parse_step(xpath_ast_node* set)
        {
            if (set && set->rettype() != xpath_type_node_set)
                throw_error("Step has to be applied to node set");

            bool axis_specified = false;
            axis_t axis = axis_child; // implied child axis

            if (_lexer.current() == lex_axis_attribute)
            {
                axis = axis_attribute;
                axis_specified = true;
                
                _lexer.next();
            }
            else if (_lexer.current() == lex_dot)
            {
                _lexer.next();
                
                return new (alloc_node()) xpath_ast_node(ast_step, set, axis_self, nodetest_type_node, 0);
            }
            else if (_lexer.current() == lex_double_dot)
            {
                _lexer.next();
                
                return new (alloc_node()) xpath_ast_node(ast_step, set, axis_parent, nodetest_type_node, 0);
            }
        
            nodetest_t nt_type = nodetest_none;
            xpath_lexer_string nt_name;
            
            if (_lexer.current() == lex_string)
            {
                // node name test
                nt_name = _lexer.contents();
                _lexer.next();

                // was it an axis name?
                if (_lexer.current() == lex_double_colon)
                {
                    // parse axis name
                    if (axis_specified) throw_error("Two axis specifiers in one step");

                    axis = parse_axis_name(nt_name, axis_specified);

                    if (!axis_specified) throw_error("Unknown axis");

                    // read actual node test
                    _lexer.next();

                    if (_lexer.current() == lex_multiply)
                    {
                        nt_type = nodetest_all;
                        nt_name = xpath_lexer_string();
                        _lexer.next();
                    }
                    else if (_lexer.current() == lex_string)
                    {
                        nt_name = _lexer.contents();
                        _lexer.next();
                    }
                    else throw_error("Unrecognized node test");
                }
                
                if (nt_type == nodetest_none)
                {
                    // node type test or processing-instruction
                    if (_lexer.current() == lex_open_brace)
                    {
                        _lexer.next();
                        
                        if (_lexer.current() == lex_close_brace)
                        {
                            _lexer.next();

                            nt_type = parse_node_test_type(nt_name);

                            if (nt_type == nodetest_none) throw_error("Unrecognized node type");
                            
                            nt_name = xpath_lexer_string();
                        }
                        else if (nt_name == PUGIXML_TEXT("processing-instruction"))
                        {
                            if (_lexer.current() != lex_quoted_string)
                                throw_error("Only literals are allowed as arguments to processing-instruction()");
                        
                            nt_type = nodetest_pi;
                            nt_name = _lexer.contents();
                            _lexer.next();
                            
                            if (_lexer.current() != lex_close_brace)
                                throw_error("Unmatched brace near processing-instruction()");
                            _lexer.next();
                        }
                        else
                            throw_error("Unmatched brace near node type test");

                    }
                    // QName or NCName:*
                    else
                    {
                        if (nt_name.end - nt_name.begin > 2 && nt_name.end[-2] == ':' && nt_name.end[-1] == '*') // NCName:*
                        {
                            nt_name.end--; // erase *
                            
                            nt_type = nodetest_all_in_namespace;
                        }
                        else nt_type = nodetest_name;
                    }
                }
            }
            else if (_lexer.current() == lex_multiply)
            {
                nt_type = nodetest_all;
                _lexer.next();
            }
            else throw_error("Unrecognized node test");
            
            xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step, set, axis, nt_type, alloc_string(nt_name));
            
            xpath_ast_node* last = 0;
            
            while (_lexer.current() == lex_open_square_brace)
            {
                _lexer.next();
                
                xpath_ast_node* expr = parse_expression();

                xpath_ast_node* pred = new (alloc_node()) xpath_ast_node(ast_predicate, 0, expr, predicate_default);
                
                if (_lexer.current() != lex_close_square_brace)
                    throw_error("Unmatched square brace");
                _lexer.next();
                
                if (last) last->set_next(pred);
                else n->set_right(pred);
                
                last = pred;
            }

            return n;
        }
        
        // RelativeLocationPath ::= Step | RelativeLocationPath '/' Step | RelativeLocationPath '//' Step
        xpath_ast_node* parse_relative_location_path(xpath_ast_node* set)
        {
            xpath_ast_node* n = parse_step(set);
            
            while (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
            {
                lexeme_t l = _lexer.current();
                _lexer.next();

                if (l == lex_double_slash)
                    n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
                
                n = parse_step(n);
            }
            
            return n;
        }
        
        // LocationPath ::= RelativeLocationPath | AbsoluteLocationPath
        // AbsoluteLocationPath ::= '/' RelativeLocationPath? | '//' RelativeLocationPath
        xpath_ast_node* parse_location_path()
        {
            if (_lexer.current() == lex_slash)
            {
                _lexer.next();
                
                xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpath_type_node_set);

                // relative location path can start from axis_attribute, dot, double_dot, multiply and string lexemes; any other lexeme means standalone root path
                lexeme_t l = _lexer.current();

                if (l == lex_string || l == lex_axis_attribute || l == lex_dot || l == lex_double_dot || l == lex_multiply)
                    return parse_relative_location_path(n);
                else
                    return n;
            }
            else if (_lexer.current() == lex_double_slash)
            {
                _lexer.next();
                
                xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpath_type_node_set);
                n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
                
                return parse_relative_location_path(n);
            }

            // else clause moved outside of if because of bogus warning 'control may reach end of non-void function being inlined' in gcc 4.0.1
            return parse_relative_location_path(0);
        }
        
        // PathExpr ::= LocationPath
        //                | FilterExpr
        //                | FilterExpr '/' RelativeLocationPath
        //                | FilterExpr '//' RelativeLocationPath
        // UnionExpr ::= PathExpr | UnionExpr '|' PathExpr
        // UnaryExpr ::= UnionExpr | '-' UnaryExpr
        xpath_ast_node* parse_path_or_unary_expression()
        {
            // Clarification.
            // PathExpr begins with either LocationPath or FilterExpr.
            // FilterExpr begins with PrimaryExpr
            // PrimaryExpr begins with '$' in case of it being a variable reference,
            // '(' in case of it being an expression, string literal, number constant or
            // function call.

            if (_lexer.current() == lex_var_ref || _lexer.current() == lex_open_brace || 
                _lexer.current() == lex_quoted_string || _lexer.current() == lex_number ||
                _lexer.current() == lex_string)
            {
                if (_lexer.current() == lex_string)
                {
                    // This is either a function call, or not - if not, we shall proceed with location path
                    const char_t* state = _lexer.state();
                    
                    while (PUGI__IS_CHARTYPE(*state, ct_space)) ++state;
                    
                    if (*state != '(') return parse_location_path();

                    // This looks like a function call; however this still can be a node-test. Check it.
                    if (parse_node_test_type(_lexer.contents()) != nodetest_none) return parse_location_path();
                }
                
                xpath_ast_node* n = parse_filter_expression();

                if (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
                {
                    lexeme_t l = _lexer.current();
                    _lexer.next();
                    
                    if (l == lex_double_slash)
                    {
                        if (n->rettype() != xpath_type_node_set) throw_error("Step has to be applied to node set");

                        n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
                    }
    
                    // select from location path
                    return parse_relative_location_path(n);
                }

                return n;
            }
            else if (_lexer.current() == lex_minus)
            {
                _lexer.next();

                // precedence 7+ - only parses union expressions
                xpath_ast_node* expr = parse_expression_rec(parse_path_or_unary_expression(), 7);

                return new (alloc_node()) xpath_ast_node(ast_op_negate, xpath_type_number, expr);
            }
            else
                return parse_location_path();
        }

        struct binary_op_t
        {
            ast_type_t asttype;
            xpath_value_type rettype;
            int precedence;

            binary_op_t(): asttype(ast_unknown), rettype(xpath_type_none), precedence(0)
            {
            }

            binary_op_t(ast_type_t asttype_, xpath_value_type rettype_, int precedence_): asttype(asttype_), rettype(rettype_), precedence(precedence_)
            {
            }

            static binary_op_t parse(xpath_lexer& lexer)
            {
                switch (lexer.current())
                {
                case lex_string:
                    if (lexer.contents() == PUGIXML_TEXT("or"))
                        return binary_op_t(ast_op_or, xpath_type_boolean, 1);
                    else if (lexer.contents() == PUGIXML_TEXT("and"))
                        return binary_op_t(ast_op_and, xpath_type_boolean, 2);
                    else if (lexer.contents() == PUGIXML_TEXT("div"))
                        return binary_op_t(ast_op_divide, xpath_type_number, 6);
                    else if (lexer.contents() == PUGIXML_TEXT("mod"))
                        return binary_op_t(ast_op_mod, xpath_type_number, 6);
                    else
                        return binary_op_t();

                case lex_equal:
                    return binary_op_t(ast_op_equal, xpath_type_boolean, 3);

                case lex_not_equal:
                    return binary_op_t(ast_op_not_equal, xpath_type_boolean, 3);

                case lex_less:
                    return binary_op_t(ast_op_less, xpath_type_boolean, 4);

                case lex_greater:
                    return binary_op_t(ast_op_greater, xpath_type_boolean, 4);

                case lex_less_or_equal:
                    return binary_op_t(ast_op_less_or_equal, xpath_type_boolean, 4);

                case lex_greater_or_equal:
                    return binary_op_t(ast_op_greater_or_equal, xpath_type_boolean, 4);

                case lex_plus:
                    return binary_op_t(ast_op_add, xpath_type_number, 5);

                case lex_minus:
                    return binary_op_t(ast_op_subtract, xpath_type_number, 5);

                case lex_multiply:
                    return binary_op_t(ast_op_multiply, xpath_type_number, 6);

                case lex_union:
                    return binary_op_t(ast_op_union, xpath_type_node_set, 7);

                default:
                    return binary_op_t();
                }
            }
        };

        xpath_ast_node* parse_expression_rec(xpath_ast_node* lhs, int limit)
        {
            binary_op_t op = binary_op_t::parse(_lexer);

            while (op.asttype != ast_unknown && op.precedence >= limit)
            {
                _lexer.next();

                xpath_ast_node* rhs = parse_path_or_unary_expression();

                binary_op_t nextop = binary_op_t::parse(_lexer);

                while (nextop.asttype != ast_unknown && nextop.precedence > op.precedence)
                {
                    rhs = parse_expression_rec(rhs, nextop.precedence);

                    nextop = binary_op_t::parse(_lexer);
                }

                if (op.asttype == ast_op_union && (lhs->rettype() != xpath_type_node_set || rhs->rettype() != xpath_type_node_set))
                    throw_error("Union operator has to be applied to node sets");

                lhs = new (alloc_node()) xpath_ast_node(op.asttype, op.rettype, lhs, rhs);

                op = binary_op_t::parse(_lexer);
            }

            return lhs;
        }

        // Expr ::= OrExpr
        // OrExpr ::= AndExpr | OrExpr 'or' AndExpr
        // AndExpr ::= EqualityExpr | AndExpr 'and' EqualityExpr
        // EqualityExpr ::= RelationalExpr
        //                    | EqualityExpr '=' RelationalExpr
        //                    | EqualityExpr '!=' RelationalExpr
        // RelationalExpr ::= AdditiveExpr
        //                      | RelationalExpr '<' AdditiveExpr
        //                      | RelationalExpr '>' AdditiveExpr
        //                      | RelationalExpr '<=' AdditiveExpr
        //                      | RelationalExpr '>=' AdditiveExpr
        // AdditiveExpr ::= MultiplicativeExpr
        //                    | AdditiveExpr '+' MultiplicativeExpr
        //                    | AdditiveExpr '-' MultiplicativeExpr
        // MultiplicativeExpr ::= UnaryExpr
        //                          | MultiplicativeExpr '*' UnaryExpr
        //                          | MultiplicativeExpr 'div' UnaryExpr
        //                          | MultiplicativeExpr 'mod' UnaryExpr
        xpath_ast_node* parse_expression()
        {
            return parse_expression_rec(parse_path_or_unary_expression(), 0);
        }

        xpath_parser(const char_t* query, xpath_variable_set* variables, xpath_allocator* alloc, xpath_parse_result* result): _alloc(alloc), _lexer(query), _query(query), _variables(variables), _result(result)
        {
        }

        xpath_ast_node* parse()
        {
            xpath_ast_node* result = parse_expression();
            
            if (_lexer.current() != lex_eof)
            {
                // there are still unparsed tokens left, error
                throw_error("Incorrect query");
            }
            
            return result;
        }

        static xpath_ast_node* parse(const char_t* query, xpath_variable_set* variables, xpath_allocator* alloc, xpath_parse_result* result)
        {
            xpath_parser parser(query, variables, alloc, result);

        #ifdef PUGIXML_NO_EXCEPTIONS
            int error = setjmp(parser._error_handler);

            return (error == 0) ? parser.parse() : 0;
        #else
            return parser.parse();
        #endif
        }
    };

    struct xpath_query_impl
    {
        static xpath_query_impl* create()
        {
            void* memory = xml_memory::allocate(sizeof(xpath_query_impl));

            return new (memory) xpath_query_impl();
        }

        static void destroy(void* ptr)
        {
            if (!ptr) return;
            
            // free all allocated pages
            static_cast<xpath_query_impl*>(ptr)->alloc.release();

            // free allocator memory (with the first page)
            xml_memory::deallocate(ptr);
        }

        xpath_query_impl(): root(0), alloc(&block)
        {
            block.next = 0;
            block.capacity = sizeof(block.data);
        }

        xpath_ast_node* root;
        xpath_allocator alloc;
        xpath_memory_block block;
    };

    PUGI__FN xpath_string evaluate_string_impl(xpath_query_impl* impl, const xpath_node& n, xpath_stack_data& sd)
    {
        if (!impl) return xpath_string();

    #ifdef PUGIXML_NO_EXCEPTIONS
        if (setjmp(sd.error_handler)) return xpath_string();
    #endif

        xpath_context c(n, 1, 1);

        return impl->root->eval_string(c, sd.stack);
    }

    PUGI__FN impl::xpath_ast_node* evaluate_node_set_prepare(xpath_query_impl* impl)
    {
        if (!impl) return 0;

        if (impl->root->rettype() != xpath_type_node_set)
        {
        #ifdef PUGIXML_NO_EXCEPTIONS
            return 0;
        #else
            xpath_parse_result res;
            res.error = "Expression does not evaluate to node set";

            throw xpath_exception(res);
        #endif
        }

        return impl->root;
    }
PUGI__NS_END

namespace pugi
{
#ifndef PUGIXML_NO_EXCEPTIONS
    PUGI__FN xpath_exception::xpath_exception(const xpath_parse_result& result_): _result(result_)
    {
        assert(_result.error);
    }
    
    PUGI__FN const char* xpath_exception::what() const throw()
    {
        return _result.error;
    }

    PUGI__FN const xpath_parse_result& xpath_exception::result() const
    {
        return _result;
    }
#endif
    
    PUGI__FN xpath_node::xpath_node()
    {
    }
        
    PUGI__FN xpath_node::xpath_node(const xml_node& node_): _node(node_)
    {
    }
        
    PUGI__FN xpath_node::xpath_node(const xml_attribute& attribute_, const xml_node& parent_): _node(attribute_ ? parent_ : xml_node()), _attribute(attribute_)
    {
    }

    PUGI__FN xml_node xpath_node::node() const
    {
        return _attribute ? xml_node() : _node;
    }
        
    PUGI__FN xml_attribute xpath_node::attribute() const
    {
        return _attribute;
    }
    
    PUGI__FN xml_node xpath_node::parent() const
    {
        return _attribute ? _node : _node.parent();
    }

    PUGI__FN static void unspecified_bool_xpath_node(xpath_node***)
    {
    }

    PUGI__FN xpath_node::operator xpath_node::unspecified_bool_type() const
    {
        return (_node || _attribute) ? unspecified_bool_xpath_node : 0;
    }
    
    PUGI__FN bool xpath_node::operator!() const
    {
        return !(_node || _attribute);
    }

    PUGI__FN bool xpath_node::operator==(const xpath_node& n) const
    {
        return _node == n._node && _attribute == n._attribute;
    }
    
    PUGI__FN bool xpath_node::operator!=(const xpath_node& n) const
    {
        return _node != n._node || _attribute != n._attribute;
    }

#ifdef __BORLANDC__
    PUGI__FN bool operator&&(const xpath_node& lhs, bool rhs)
    {
        return (bool)lhs && rhs;
    }

    PUGI__FN bool operator||(const xpath_node& lhs, bool rhs)
    {
        return (bool)lhs || rhs;
    }
#endif

    PUGI__FN void xpath_node_set::_assign(const_iterator begin_, const_iterator end_)
    {
        assert(begin_ <= end_);

        size_t size_ = static_cast<size_t>(end_ - begin_);

        if (size_ <= 1)
        {
            // deallocate old buffer
            if (_begin != &_storage) impl::xml_memory::deallocate(_begin);

            // use internal buffer
            if (begin_ != end_) _storage = *begin_;

            _begin = &_storage;
            _end = &_storage + size_;
        }
        else
        {
            // make heap copy
            xpath_node* storage = static_cast<xpath_node*>(impl::xml_memory::allocate(size_ * sizeof(xpath_node)));

            if (!storage)
            {
            #ifdef PUGIXML_NO_EXCEPTIONS
                return;
            #else
                throw std::bad_alloc();
            #endif
            }

            memcpy(storage, begin_, size_ * sizeof(xpath_node));
            
            // deallocate old buffer
            if (_begin != &_storage) impl::xml_memory::deallocate(_begin);

            // finalize
            _begin = storage;
            _end = storage + size_;
        }
    }

    PUGI__FN xpath_node_set::xpath_node_set(): _type(type_unsorted), _begin(&_storage), _end(&_storage)
    {
    }

    PUGI__FN xpath_node_set::xpath_node_set(const_iterator begin_, const_iterator end_, type_t type_): _type(type_), _begin(&_storage), _end(&_storage)
    {
        _assign(begin_, end_);
    }

    PUGI__FN xpath_node_set::~xpath_node_set()
    {
        if (_begin != &_storage) impl::xml_memory::deallocate(_begin);
    }
        
    PUGI__FN xpath_node_set::xpath_node_set(const xpath_node_set& ns): _type(ns._type), _begin(&_storage), _end(&_storage)
    {
        _assign(ns._begin, ns._end);
    }
    
    PUGI__FN xpath_node_set& xpath_node_set::operator=(const xpath_node_set& ns)
    {
        if (this == &ns) return *this;
        
        _type = ns._type;
        _assign(ns._begin, ns._end);

        return *this;
    }

    PUGI__FN xpath_node_set::type_t xpath_node_set::type() const
    {
        return _type;
    }
        
    PUGI__FN size_t xpath_node_set::size() const
    {
        return _end - _begin;
    }
        
    PUGI__FN bool xpath_node_set::empty() const
    {
        return _begin == _end;
    }
        
    PUGI__FN const xpath_node& xpath_node_set::operator[](size_t index) const
    {
        assert(index < size());
        return _begin[index];
    }

    PUGI__FN xpath_node_set::const_iterator xpath_node_set::begin() const
    {
        return _begin;
    }
        
    PUGI__FN xpath_node_set::const_iterator xpath_node_set::end() const
    {
        return _end;
    }
    
    PUGI__FN void xpath_node_set::sort(bool reverse)
    {
        _type = impl::xpath_sort(_begin, _end, _type, reverse);
    }

    PUGI__FN xpath_node xpath_node_set::first() const
    {
        return impl::xpath_first(_begin, _end, _type);
    }

    PUGI__FN xpath_parse_result::xpath_parse_result(): error("Internal error"), offset(0)
    {
    }

    PUGI__FN xpath_parse_result::operator bool() const
    {
        return error == 0;
    }

    PUGI__FN const char* xpath_parse_result::description() const
    {
        return error ? error : "No error";
    }

    PUGI__FN xpath_variable::xpath_variable(): _type(xpath_type_none), _next(0)
    {
    }

    PUGI__FN const char_t* xpath_variable::name() const
    {
        switch (_type)
        {
        case xpath_type_node_set:
            return static_cast<const impl::xpath_variable_node_set*>(this)->name;

        case xpath_type_number:
            return static_cast<const impl::xpath_variable_number*>(this)->name;

        case xpath_type_string:
            return static_cast<const impl::xpath_variable_string*>(this)->name;

        case xpath_type_boolean:
            return static_cast<const impl::xpath_variable_boolean*>(this)->name;

        default:
            assert(!"Invalid variable type");
            return 0;
        }
    }

    PUGI__FN xpath_value_type xpath_variable::type() const
    {
        return _type;
    }

    PUGI__FN bool xpath_variable::get_boolean() const
    {
        return (_type == xpath_type_boolean) ? static_cast<const impl::xpath_variable_boolean*>(this)->value : false;
    }

    PUGI__FN double xpath_variable::get_number() const
    {
        return (_type == xpath_type_number) ? static_cast<const impl::xpath_variable_number*>(this)->value : impl::gen_nan();
    }

    PUGI__FN const char_t* xpath_variable::get_string() const
    {
        const char_t* value = (_type == xpath_type_string) ? static_cast<const impl::xpath_variable_string*>(this)->value : 0;
        return value ? value : PUGIXML_TEXT("");
    }

    PUGI__FN const xpath_node_set& xpath_variable::get_node_set() const
    {
        return (_type == xpath_type_node_set) ? static_cast<const impl::xpath_variable_node_set*>(this)->value : impl::dummy_node_set;
    }

    PUGI__FN bool xpath_variable::set(bool value)
    {
        if (_type != xpath_type_boolean) return false;

        static_cast<impl::xpath_variable_boolean*>(this)->value = value;
        return true;
    }

    PUGI__FN bool xpath_variable::set(double value)
    {
        if (_type != xpath_type_number) return false;

        static_cast<impl::xpath_variable_number*>(this)->value = value;
        return true;
    }

    PUGI__FN bool xpath_variable::set(const char_t* value)
    {
        if (_type != xpath_type_string) return false;

        impl::xpath_variable_string* var = static_cast<impl::xpath_variable_string*>(this);

        // duplicate string
        size_t size = (impl::strlength(value) + 1) * sizeof(char_t);

        char_t* copy = static_cast<char_t*>(impl::xml_memory::allocate(size));
        if (!copy) return false;

        memcpy(copy, value, size);

        // replace old string
        if (var->value) impl::xml_memory::deallocate(var->value);
        var->value = copy;

        return true;
    }

    PUGI__FN bool xpath_variable::set(const xpath_node_set& value)
    {
        if (_type != xpath_type_node_set) return false;

        static_cast<impl::xpath_variable_node_set*>(this)->value = value;
        return true;
    }

    PUGI__FN xpath_variable_set::xpath_variable_set()
    {
        for (size_t i = 0; i < sizeof(_data) / sizeof(_data[0]); ++i) _data[i] = 0;
    }

    PUGI__FN xpath_variable_set::~xpath_variable_set()
    {
        for (size_t i = 0; i < sizeof(_data) / sizeof(_data[0]); ++i)
        {
            xpath_variable* var = _data[i];

            while (var)
            {
                xpath_variable* next = var->_next;

                impl::delete_xpath_variable(var->_type, var);

                var = next;
            }
        }
    }

    PUGI__FN xpath_variable* xpath_variable_set::find(const char_t* name) const
    {
        const size_t hash_size = sizeof(_data) / sizeof(_data[0]);
        size_t hash = impl::hash_string(name) % hash_size;

        // look for existing variable
        for (xpath_variable* var = _data[hash]; var; var = var->_next)
            if (impl::strequal(var->name(), name))
                return var;

        return 0;
    }

    PUGI__FN xpath_variable* xpath_variable_set::add(const char_t* name, xpath_value_type type)
    {
        const size_t hash_size = sizeof(_data) / sizeof(_data[0]);
        size_t hash = impl::hash_string(name) % hash_size;

        // look for existing variable
        for (xpath_variable* var = _data[hash]; var; var = var->_next)
            if (impl::strequal(var->name(), name))
                return var->type() == type ? var : 0;

        // add new variable
        xpath_variable* result = impl::new_xpath_variable(type, name);

        if (result)
        {
            result->_type = type;
            result->_next = _data[hash];

            _data[hash] = result;
        }

        return result;
    }

    PUGI__FN bool xpath_variable_set::set(const char_t* name, bool value)
    {
        xpath_variable* var = add(name, xpath_type_boolean);
        return var ? var->set(value) : false;
    }

    PUGI__FN bool xpath_variable_set::set(const char_t* name, double value)
    {
        xpath_variable* var = add(name, xpath_type_number);
        return var ? var->set(value) : false;
    }

    PUGI__FN bool xpath_variable_set::set(const char_t* name, const char_t* value)
    {
        xpath_variable* var = add(name, xpath_type_string);
        return var ? var->set(value) : false;
    }

    PUGI__FN bool xpath_variable_set::set(const char_t* name, const xpath_node_set& value)
    {
        xpath_variable* var = add(name, xpath_type_node_set);
        return var ? var->set(value) : false;
    }

    PUGI__FN xpath_variable* xpath_variable_set::get(const char_t* name)
    {
        return find(name);
    }

    PUGI__FN const xpath_variable* xpath_variable_set::get(const char_t* name) const
    {
        return find(name);
    }

    PUGI__FN xpath_query::xpath_query(const char_t* query, xpath_variable_set* variables): _impl(0)
    {
        impl::xpath_query_impl* qimpl = impl::xpath_query_impl::create();

        if (!qimpl)
        {
        #ifdef PUGIXML_NO_EXCEPTIONS
            _result.error = "Out of memory";
        #else
            throw std::bad_alloc();
        #endif
        }
        else
        {
            impl::buffer_holder impl_holder(qimpl, impl::xpath_query_impl::destroy);

            qimpl->root = impl::xpath_parser::parse(query, variables, &qimpl->alloc, &_result);

            if (qimpl->root)
            {
                qimpl->root->optimize(&qimpl->alloc);

                _impl = static_cast<impl::xpath_query_impl*>(impl_holder.release());
                _result.error = 0;
            }
        }
    }

    PUGI__FN xpath_query::~xpath_query()
    {
        impl::xpath_query_impl::destroy(_impl);
    }

    PUGI__FN xpath_value_type xpath_query::return_type() const
    {
        if (!_impl) return xpath_type_none;

        return static_cast<impl::xpath_query_impl*>(_impl)->root->rettype();
    }

    PUGI__FN bool xpath_query::evaluate_boolean(const xpath_node& n) const
    {
        if (!_impl) return false;
        
        impl::xpath_context c(n, 1, 1);
        impl::xpath_stack_data sd;

    #ifdef PUGIXML_NO_EXCEPTIONS
        if (setjmp(sd.error_handler)) return false;
    #endif
        
        return static_cast<impl::xpath_query_impl*>(_impl)->root->eval_boolean(c, sd.stack);
    }
    
    PUGI__FN double xpath_query::evaluate_number(const xpath_node& n) const
    {
        if (!_impl) return impl::gen_nan();
        
        impl::xpath_context c(n, 1, 1);
        impl::xpath_stack_data sd;

    #ifdef PUGIXML_NO_EXCEPTIONS
        if (setjmp(sd.error_handler)) return impl::gen_nan();
    #endif

        return static_cast<impl::xpath_query_impl*>(_impl)->root->eval_number(c, sd.stack);
    }

#ifndef PUGIXML_NO_STL
    PUGI__FN string_t xpath_query::evaluate_string(const xpath_node& n) const
    {
        impl::xpath_stack_data sd;

        impl::xpath_string r = impl::evaluate_string_impl(static_cast<impl::xpath_query_impl*>(_impl), n, sd);

        return string_t(r.c_str(), r.length());
    }
#endif

    PUGI__FN size_t xpath_query::evaluate_string(char_t* buffer, size_t capacity, const xpath_node& n) const
    {
        impl::xpath_stack_data sd;

        impl::xpath_string r = impl::evaluate_string_impl(static_cast<impl::xpath_query_impl*>(_impl), n, sd);

        size_t full_size = r.length() + 1;
        
        if (capacity > 0)
        {
            size_t size = (full_size < capacity) ? full_size : capacity;
            assert(size > 0);

            memcpy(buffer, r.c_str(), (size - 1) * sizeof(char_t));
            buffer[size - 1] = 0;
        }
        
        return full_size;
    }

    PUGI__FN xpath_node_set xpath_query::evaluate_node_set(const xpath_node& n) const
    {
        impl::xpath_ast_node* root = impl::evaluate_node_set_prepare(static_cast<impl::xpath_query_impl*>(_impl));
        if (!root) return xpath_node_set();

        impl::xpath_context c(n, 1, 1);
        impl::xpath_stack_data sd;

    #ifdef PUGIXML_NO_EXCEPTIONS
        if (setjmp(sd.error_handler)) return xpath_node_set();
    #endif

        impl::xpath_node_set_raw r = root->eval_node_set(c, sd.stack, impl::nodeset_eval_all);

        return xpath_node_set(r.begin(), r.end(), r.type());
    }

    PUGI__FN xpath_node xpath_query::evaluate_node(const xpath_node& n) const
    {
        impl::xpath_ast_node* root = impl::evaluate_node_set_prepare(static_cast<impl::xpath_query_impl*>(_impl));
        if (!root) return xpath_node();

        impl::xpath_context c(n, 1, 1);
        impl::xpath_stack_data sd;

    #ifdef PUGIXML_NO_EXCEPTIONS
        if (setjmp(sd.error_handler)) return xpath_node();
    #endif

        impl::xpath_node_set_raw r = root->eval_node_set(c, sd.stack, impl::nodeset_eval_first);

        return r.first();
    }

    PUGI__FN const xpath_parse_result& xpath_query::result() const
    {
        return _result;
    }

    PUGI__FN static void unspecified_bool_xpath_query(xpath_query***)
    {
    }

    PUGI__FN xpath_query::operator xpath_query::unspecified_bool_type() const
    {
        return _impl ? unspecified_bool_xpath_query : 0;
    }

    PUGI__FN bool xpath_query::operator!() const
    {
        return !_impl;
    }

    PUGI__FN xpath_node xml_node::select_node(const char_t* query, xpath_variable_set* variables) const
    {
        xpath_query q(query, variables);
        return select_node(q);
    }

    PUGI__FN xpath_node xml_node::select_node(const xpath_query& query) const
    {
        return query.evaluate_node(*this);
    }

    PUGI__FN xpath_node_set xml_node::select_nodes(const char_t* query, xpath_variable_set* variables) const
    {
        xpath_query q(query, variables);
        return select_nodes(q);
    }

    PUGI__FN xpath_node_set xml_node::select_nodes(const xpath_query& query) const
    {
        return query.evaluate_node_set(*this);
    }

    PUGI__FN xpath_node xml_node::select_single_node(const char_t* query, xpath_variable_set* variables) const
    {
        xpath_query q(query, variables);
        return select_single_node(q);
    }

    PUGI__FN xpath_node xml_node::select_single_node(const xpath_query& query) const
    {
        return query.evaluate_node(*this);
    }
}

#endif

#ifdef __BORLANDC__
#    pragma option pop
#endif

// Intel C++ does not properly keep warning state for function templates,
// so popping warning state at the end of translation unit leads to warnings in the middle.
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#    pragma warning(pop)
#endif

// Undefine all local macros (makes sure we're not leaking macros in header-only mode)
#undef PUGI__NO_INLINE
#undef PUGI__UNLIKELY
#undef PUGI__STATIC_ASSERT
#undef PUGI__DMC_VOLATILE
#undef PUGI__MSVC_CRT_VERSION
#undef PUGI__NS_BEGIN
#undef PUGI__NS_END
#undef PUGI__FN
#undef PUGI__FN_NO_INLINE
#undef PUGI__NODETYPE
#undef PUGI__IS_CHARTYPE_IMPL
#undef PUGI__IS_CHARTYPE
#undef PUGI__IS_CHARTYPEX
#undef PUGI__ENDSWITH
#undef PUGI__SKIPWS
#undef PUGI__OPTSET
#undef PUGI__PUSHNODE
#undef PUGI__POPNODE
#undef PUGI__SCANFOR
#undef PUGI__SCANWHILE
#undef PUGI__SCANWHILE_UNROLL
#undef PUGI__ENDSEG
#undef PUGI__THROW_ERROR
#undef PUGI__CHECK_ERROR

#endif

/**
 * Copyright (c) 2006-2015 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
