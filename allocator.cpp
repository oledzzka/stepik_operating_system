#include <iostream>
// Эта функция будет вызвана перед тем как вызывать myalloc и myfree
    // используйте ее чтобы инициализировать ваш аллокатор перед началом
    // работы.
    //
    // buf - указатель на участок логической памяти, который ваш аллокатор
    //       должен распределять, все возвращаемые указатели должны быть
    //       либо равны NULL, либо быть из этого участка памяти
    // size - размер участка памяти, на который указывает buf
    namespace allocator {    
        void * head;
        std::size_t size = 0;
        std::size_t border_tag_size = sizeof(bool) + sizeof(std::size_t);
    };
    

  void mysetup(void *buf, std::size_t size)
  {
        allocator::head = buf;
        allocator::size = size;
        using namespace allocator;
        bool* start_free = static_cast<bool*>(head);
        *start_free = true;
        size_t* start_size = (size_t*)(start_free + 1);
        *start_size = allocator::size - 2*allocator::border_tag_size;
        size_t* end_size = (size_t*)((char*)allocator::head + allocator::border_tag_size + *start_size);
        *end_size = *start_size;
        bool* end_free = (bool*)((char*)allocator::head + allocator::size - 1);
        *end_free = true;
    }

    // Функция аллокации
  void *myalloc(std::size_t size)
  {
        bool * pointer_free = static_cast<bool*>(allocator::head);
        size_t* pointer_size = static_cast<size_t*>((void*)((char*)allocator::head + sizeof(bool)));
        while (pointer_free != nullptr)
        {
            if (*pointer_free && *pointer_size >= size)
            {
                if (*pointer_size > size + 2 * allocator::border_tag_size) {
                    bool* end_pointer_free = (bool*)((char*)pointer_size + *pointer_size + 2*sizeof(std::size_t));
                    std::size_t* end_pointer_size = (std::size_t*)((char*)pointer_size + *pointer_size + sizeof(std::size_t));
                    *end_pointer_size = size;
                    *end_pointer_free = false;
                    std::size_t* start_pointer_size = (std::size_t*)((char*)end_pointer_size - size - sizeof(std::size_t));
                    bool* start_pointer_free = (bool*)((char*) start_pointer_size - sizeof(bool));
                    *start_pointer_free = false;
                    *start_pointer_size = size;
                    bool* new_end_pointer_free = (bool*)((char*)start_pointer_free - sizeof(bool));
                    *new_end_pointer_free = true;
                    std::size_t* new_end_pointer_size = (std::size_t*)((char*) new_end_pointer_free - sizeof(std::size_t*));
                    *new_end_pointer_size = *pointer_size - 2*allocator::border_tag_size - size;
                    *pointer_size = *new_end_pointer_size;
                    return (void*)(start_pointer_size + 1);
                } else {
                    *pointer_free = false;
                    bool* end_pointer_free = (bool*)((char*)pointer_size + *pointer_size + 2*sizeof(std::size_t));
                    *end_pointer_free = false;
                    return (void*)(pointer_size + 1);
                }
            } else {
                
                if ((char*)pointer_size + sizeof(size_t) + *pointer_size + 3*allocator::border_tag_size + size<= (char*)allocator::head + allocator::size) {
                    pointer_size = (std::size_t*)((char*)pointer_size + *pointer_size + 2*allocator::border_tag_size);
                    pointer_free = (bool*)(pointer_size - sizeof(bool));
                } else break;
            }
        }
        return nullptr;
    }

    // Функция освобождения
  void myfree(void *p)
  {
        std::size_t* pointer_size = (std::size_t*)((char*)p - sizeof(std::size_t));
        bool* pointer_start_free = (bool*)((char*)pointer_size - sizeof(bool));
        *pointer_start_free = true;
        std::size_t* pointer_end_size = (std::size_t*)((char*)p  + *pointer_size);
        bool* pointer_end_free = (bool*)((char*)p + *pointer_size + sizeof(std::size_t)); 
        *pointer_end_free = true;
        bool* left_end_pointer_buddy_free = pointer_start_free - 1;
        bool* right_start_pointer_buddy_free = (bool*)((char*)p + allocator::border_tag_size + *pointer_size); 
        if (left_end_pointer_buddy_free>=allocator::head && *left_end_pointer_buddy_free) {
            std::size_t* left_end_pointer_buddy_size = (std::size_t*)(left_end_pointer_buddy_free - sizeof(std::size_t));
            std::size_t* left_start_pointer_buddy_size = (std::size_t*)((char*)left_end_pointer_buddy_size - *left_end_pointer_buddy_size - sizeof(std::size_t));
            bool* left_start_pointer_buddy_free = (bool*)((char*)left_start_pointer_buddy_size - sizeof(bool));
            *left_start_pointer_buddy_size += *pointer_size + 2*allocator::border_tag_size;
            *left_start_pointer_buddy_free = true;
            pointer_size = left_start_pointer_buddy_size;
            *pointer_end_size = *pointer_size;
            pointer_start_free = left_start_pointer_buddy_free;
        }
        if (right_start_pointer_buddy_free<(void*)((char*)allocator::head + allocator::size) && *right_start_pointer_buddy_free) {
            std::size_t* right_start_pointer_buddy_size = (std::size_t*)(right_start_pointer_buddy_free + 1);
            std::size_t* right_end_pointer_buddy_size = (std::size_t*)((char*)right_start_pointer_buddy_size + *right_start_pointer_buddy_size + sizeof(std::size_t));
            *right_end_pointer_buddy_size += *pointer_size + 2*allocator::border_tag_size;
            *pointer_size = *right_end_pointer_buddy_size;
        }
    }

// int main(int argc, char const *argv[])
// {
//     void* a[120];
//     void *buf = std::malloc(200);
//     mysetup(buf, 200);
//     int end = 0;
//     for(int i = 0; i < 122; i++)
//     {
//         void* s = myalloc(16);
//         a[i] = s;
//         if (s == nullptr) {
//             end = i;
//             char * point = (char*)((char*)allocator::head + allocator::size - allocator::border_tag_size);
//             std::cout << i;
//             break;
//         } else {
//             std::size_t num = *(std::size_t*)((char*)s - sizeof(size_t));
//             std::size_t num_end = *(std::size_t*)((char*)s + num);
//             std::cout<<num;
//         }
//     }
//     for (int i=0; i< end -1; i++) {
//         std::size_t* pointer_size = (std::size_t*)((char*)a[i] - sizeof(std::size_t));
//         std::size_t size = *pointer_size;
//         std::size_t* pointer_end_size = (std::size_t*)((char*)a[i] + *pointer_size);
//         size = *pointer_end_size;
//         std::cout<<"Урааа";    
//     }
//     std::size_t* size1 = (std::size_t*)((char*)allocator::head + 1);
//     myfree(a[4]);
//     myfree(a[0]);
//     myfree(a[1]);
//     myfree(a[2]);
//     myfree(a[3]);
//     size_t* size = (std::size_t*)((char*)allocator::head + allocator::size -1 - sizeof(std::size_t));
//     std::cout<<"Урааа";
//     std::free(buf);
//     return 0;
// }
