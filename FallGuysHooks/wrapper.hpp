#pragma once
#include <Windows.h>
#include <Psapi.h>
#include <tlhelp32.h>

#include <vector>
#include <string>
#include <unordered_map>

namespace winapi
{
#pragma region constants
    constexpr std::uint8_t max_path = MAX_PATH;
#pragma endregion constants

    struct module_info
    {
        void* base_of_dll;
        std::uint32_t size_of_image;
        void* entrypoint;
    };

    struct process_entry32
    {
        std::uint32_t size;
        std::uint32_t usage_count;
        std::uint32_t process_id;
        std::uint64_t* default_heap_id;
        std::uint32_t module_info;
        std::uint32_t thread_count;
        std::uint32_t parent_process_id;
        long class_base;
        std::uint32_t flags;
        wchar_t file[winapi::max_path];
    };

    struct security_attributes
    {
        std::uint32_t length;
        void* security_descriptor;
        bool inherit_handle;
    };

    class process
    {
    public:
#pragma region constants
        static constexpr std::uint32_t terminate = PROCESS_TERMINATE;
        static constexpr std::uint32_t create_thread = PROCESS_CREATE_THREAD;
        static constexpr std::uint32_t set_sessionid = PROCESS_SET_SESSIONID;
        static constexpr std::uint32_t vm_operation = PROCESS_VM_OPERATION;
        static constexpr std::uint32_t vm_read = PROCESS_VM_READ;
        static constexpr std::uint32_t vm_write = PROCESS_VM_WRITE;
        static constexpr std::uint32_t dup_handle = PROCESS_DUP_HANDLE;
        static constexpr std::uint32_t create_process = PROCESS_CREATE_PROCESS;
        static constexpr std::uint32_t set_quota = PROCESS_SET_QUOTA;
        static constexpr std::uint32_t set_information = PROCESS_SET_INFORMATION;
        static constexpr std::uint32_t query_information = PROCESS_QUERY_INFORMATION;
        static constexpr std::uint32_t suspend_resume = PROCESS_SUSPEND_RESUME;
        static constexpr std::uint32_t query_limited_information = PROCESS_QUERY_LIMITED_INFORMATION;
        static constexpr std::uint32_t set_limited_information = PROCESS_SET_LIMITED_INFORMATION;
        static constexpr std::uint32_t all_access = PROCESS_ALL_ACCESS;

        static constexpr uint32_t snap_heap_list = TH32CS_SNAPHEAPLIST;
        static constexpr uint32_t snap_process = TH32CS_SNAPPROCESS;
        static constexpr uint32_t snap_thread = TH32CS_SNAPTHREAD;
        static constexpr uint32_t snap_module = TH32CS_SNAPMODULE;
        static constexpr uint32_t snap_module32 = TH32CS_SNAPMODULE32;
        static constexpr uint32_t snap_all = TH32CS_SNAPALL;
        static constexpr uint32_t inherit = TH32CS_INHERIT;
#pragma endregion constants

        static inline void* open_process(
            std::uint32_t access,
            bool inherit_handle,
            std::uint32_t process_id)
        {
            return OpenProcess(
                access,
                inherit_handle,
                process_id);
        }
        static inline std::vector<std::uint32_t> enum_processes()
        {
            DWORD processes[1024];
            DWORD needed;
            if (EnumProcesses(
                processes,
                sizeof(processes),
                &needed))
            {
                int size = sizeof(processes) / sizeof(processes[0]);
                return std::vector<uint32_t>(
                    processes,
                    processes + size);
            }

            return std::vector<uint32_t>();
        }
        static inline std::wstring get_process_image_filename(
            void* process)
        {
            wchar_t file[MAX_PATH];
            GetProcessImageFileNameW(
                process,
                file,
                sizeof(file) / sizeof(file[0]));
            return std::wstring(file);
        }
        static inline void* create_toolhelp32_snapshot(
            std::uint32_t flags,
            std::uint32_t process_id)
        {
            return CreateToolhelp32Snapshot(flags, process_id);
        }
        static inline bool process32_first(
            void* snapshot,
            winapi::process_entry32* entry)
        {
            return Process32FirstW(
                snapshot,
                reinterpret_cast<PROCESSENTRY32W*>(entry));
        }
        static inline bool process32_next(
            void* snapshot,
            winapi::process_entry32* entry)
        {
            return Process32NextW(
                snapshot,
                reinterpret_cast<PROCESSENTRY32W*>(entry));
        }
        static inline void sleep(
            std::uint32_t milliseconds)
        {
            Sleep(milliseconds);
        }
        static inline void* get_current_process()
        {
            return GetCurrentProcess();
        }
        static inline bool alloc_console()
        {
            return AllocConsole();
        }
    };

    class module
    {
    public:
        static inline std::wstring get_module_file_name_ex(
            void* process,
            void* module)
        {
            wchar_t name[MAX_PATH];
            if (GetModuleFileNameExW(
                process,
                (HMODULE)module,
                name,
                sizeof(name) / sizeof(wchar_t)))
            {
                return std::wstring(name);
            }

            throw std::exception("GetModuleFileNameEx failed");
        }
        static inline winapi::module_info get_module_information(
            void* process,
            void* module)
        {
            MODULEINFO info = {};
            if (GetModuleInformation(
                process,
                (HMODULE)module,
                &info,
                sizeof(MODULEINFO)))
            {
                return module_info
                {
                    info.lpBaseOfDll,
                    info.SizeOfImage,
                    info.EntryPoint
                };
            }

            return module_info
            {
                nullptr,
                0,
                nullptr
            };
        }
        static inline std::wstring get_module_base_name(
            void* process,
            void* module)
        {
            wchar_t name[MAX_PATH] = L"";
            if (GetModuleBaseNameW(
                process,
                (HMODULE)module,
                name,
                sizeof(name) / sizeof(wchar_t)))
            {
                return std::wstring(name);
            }

            throw std::exception("GetModuleBaseName failed");
        }
        static inline std::vector<void*> enum_process_modules(
            void* process)
        {
            HMODULE modules[1024];
            DWORD needed;
            if (EnumProcessModules(
                process,
                modules,
                sizeof(modules),
                &needed))
            {
                int size = sizeof(modules) / sizeof(modules[0]);
                return std::vector<void*>(
                    modules,
                    modules + size);
            }

            return std::vector<void*>();
        }
        static inline void* get_proc_address(
            void* module,
            std::string name)
        {
            return GetProcAddress(
                (HMODULE)module,
                name.data());
        }
        static inline void* get_module_handle(
            std::wstring module)
        {
            return GetModuleHandleW(module.data());
        }
    };

    class memory
    {
    public:
#pragma region constants
        static constexpr std::uint32_t commit = MEM_COMMIT;
        static constexpr std::uint32_t reserve = MEM_RESERVE;
        static constexpr std::uint32_t reset = MEM_RESET;
        static constexpr std::uint32_t reset_undo = MEM_RESET_UNDO;
        static constexpr std::uint32_t large_pages = MEM_LARGE_PAGES;
        static constexpr std::uint32_t physical = MEM_PHYSICAL;
        static constexpr std::uint32_t top_down = MEM_TOP_DOWN;
        static constexpr std::uint32_t write_watch = MEM_WRITE_WATCH;
        static constexpr std::uint32_t release = MEM_RELEASE;

        static constexpr std::uint32_t execute = PAGE_EXECUTE;
        static constexpr std::uint32_t execute_read = PAGE_EXECUTE_READ;
        static constexpr std::uint32_t execute_read_write = PAGE_EXECUTE_READWRITE;
        static constexpr std::uint32_t execute_write_copy = PAGE_EXECUTE_WRITECOPY;
        static constexpr std::uint32_t read_write = PAGE_READWRITE;
        static constexpr std::uint32_t targets_invalid = PAGE_TARGETS_INVALID;
        static constexpr std::uint32_t targets_no_update = PAGE_TARGETS_NO_UPDATE;
        static constexpr std::uint32_t guard = PAGE_GUARD;
        static constexpr std::uint32_t no_cache = PAGE_NOCACHE;
        static constexpr std::uint32_t write_combine = PAGE_WRITECOMBINE;
#pragma endregion constants

        static inline std::vector<uint8_t> read_process_memory(
            void* process,
            void* address,
            std::size_t size)
        {
            uint8_t* buffer = (uint8_t*)malloc(size);
            SIZE_T read;
            if (ReadProcessMemory(
                process,
                address,
                buffer,
                size,
                &read))
            {
                std::vector<uint8_t> memory(buffer, buffer + read);
                free(buffer);
                return memory;
            }

            return std::vector<uint8_t>();
        }
        static inline bool write_process_memory(
            void* process,
            int address,
            std::vector<uint8_t> data)
        {
            SIZE_T written;
            return WriteProcessMemory(
                process,
                (void*)address,
                std::data(data),
                data.size(),
                &written);
        }
        static inline void* virtual_alloc(
            void* address,
            std::size_t size,
            std::uint32_t allocation_type,
            std::uint32_t protection
        )
        {
            return VirtualAlloc(
                address,
                size,
                allocation_type,
                protection);
        }
        static inline bool virtual_free(
            void* address,
            std::size_t size,
            std::uint32_t free_type)
        {
            return VirtualFree(
                address,
                size,
                free_type);
        }
        static inline bool virtual_protect(
            void* address,
            std::size_t size,
            std::uint32_t protection)
        {
            DWORD old_protection;
            return VirtualProtect(
                address,
                size,
                protection,
                &old_protection);
        }
    };

    class handle
    {
    public:
        static inline void close_handle(
            void* handle)
        {
            CloseHandle(handle);
        }
        static inline uint32_t wait_for_single_object(
            void* handle,
            uint32_t milliseconds)
        {
            return WaitForSingleObject(
                handle,
                milliseconds);
        }
    };

    class event
    {
    public:
        static inline void* open_event(
            uint32_t access,
            bool inherit_handle,
            std::wstring name)
        {
            return OpenEventW(
                access,
                inherit_handle,
                name.data());
        }
        static inline void* create_event(
            winapi::security_attributes* attributes,
            bool manual_reset,
            bool initial_state,
            std::wstring name)
        {
            return CreateEventW(
                reinterpret_cast<SECURITY_ATTRIBUTES*>(attributes),
                manual_reset,
                initial_state,
                name.data());
        }
        static inline bool set_event(
            void* event_handle)
        {
            return SetEvent(event_handle);
        }
    };

    class file
    {
    public:
        static inline void* open_file_mapping(
            std::uint32_t access,
            bool inherit_handle,
            std::wstring name)
        {
            return OpenFileMappingW(
                access,
                inherit_handle,
                name.data());
        }
        static inline void* create_file_mapping(
            void* file,
            winapi::security_attributes* attributes,
            std::uint32_t protect,
            std::uint32_t max_size_high,
            std::uint32_t max_size_low,
            std::wstring name)
        {
            return CreateFileMappingW(
                file,
                reinterpret_cast<SECURITY_ATTRIBUTES*>(attributes),
                protect,
                max_size_high,
                max_size_low,
                name.data());
        }
        static inline void* map_view_of_file(
            void* file,
            std::uint32_t access,
            std::uint32_t offset_high,
            std::uint32_t offset_low,
            std::size_t size)
        {
            return MapViewOfFile(
                file,
                access,
                offset_high,
                offset_low,
                size);
        }
        static inline bool unmap_view_of_file(
            void* file)
        {
            return UnmapViewOfFile(file);
        }
        static inline void* create_file(
            std::wstring file,
            std::uint32_t access,
            std::uint32_t shared,
            winapi::security_attributes* security_attributes,
            std::uint32_t creation_disposition,
            std::uint32_t attributes,
            void* template_file)
        {
            return CreateFileW(
                file.data(),
                access,
                shared,
                reinterpret_cast<SECURITY_ATTRIBUTES*>(security_attributes),
                creation_disposition,
                attributes,
                template_file
            );
        }
    };
}

namespace nt
{
    struct unicode_string
    {
        unsigned short length;
        unsigned short max_length;
        wchar_t* buffer;
    };

    struct system_process_information
    {
        unsigned long next_entry_offset;
        unsigned long number_of_threads;
        std::uint64_t reserved[3];
        std::uint64_t create_time;
        std::uint64_t user_time;
        std::uint64_t kernel_time;
        nt::unicode_string image_name;
        unsigned long base_priority;
        void* process_id;
        void* inherited_from_process_id;
    };

    enum system_information_class
    {
        base_information = 0,
        performance_information = 2,
        time_of_day_information = 3,
        process_information = 5,
        processor_performance_information = 8,
        interrupt_information = 23,
        exception_information = 33,
        registry_quota_information = 37,
        lookaside_information = 45
    };

    class detail
    {
        friend class system;
    private:
        static inline std::unordered_map<std::string, void*> m_ntdll = {};
    protected:
        template<class T>
        static T resolve_function(std::string name)
        {
            if (m_ntdll.find(name) == m_ntdll.end())
            {
                auto ntdll = winapi::module::get_module_handle(L"ntdll");
                auto proc = winapi::module::get_proc_address(ntdll, name);
                m_ntdll.emplace(name, proc);
            }
            return (T)m_ntdll.at(name);
        }
    };

    class system
    {
    public:
        template<class T>
        static inline std::list<T*> query_system_information()
        {
            auto NtQuerySystemInformation =
                nt::detail::resolve_function<nt_query_system_information_t>(
                    "NtQuerySystemInformation");

            if (std::is_same<
                T,
                nt::system_process_information>::value)
            {
                unsigned long length = 0;
                NtQuerySystemInformation(
                    nt::system_information_class::process_information,
                    nullptr,
                    length,
                    &length);

                auto buffer = reinterpret_cast<nt::system_process_information*>(
                    winapi::memory::virtual_alloc(
                        nullptr,
                        length,
                        winapi::memory::commit,
                        winapi::memory::read_write));

                NtQuerySystemInformation(
                    nt::system_information_class::process_information,
                    buffer,
                    length,
                    &length);

                auto proc_info = buffer;
                std::list<T*> entries;
                do
                {
                    entries.push_back(proc_info);
                    auto offset = proc_info->next_entry_offset;
                    auto next = reinterpret_cast<uintptr_t>(proc_info) + offset;
                    proc_info = reinterpret_cast<nt::system_process_information*>(
                        next);
                } while (proc_info->next_entry_offset != 0);

                winapi::memory::virtual_free(
                    buffer,
                    length,
                    winapi::memory::release);

                return entries;
            }

            return {};
        }
    private:
        using nt_query_system_information_t = NTSTATUS(NTAPI*)(
            nt::system_information_class,
            void*,
            unsigned long,
            unsigned long*);
    };
}