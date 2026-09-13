#pragma once
#include <cstdint>
#include <cstddef>

namespace il2cpp {

struct Il2CppDomain; struct Il2CppAssembly; struct Il2CppImage;
struct Il2CppClass;  struct Il2CppMethod;  struct Il2CppField;
struct Il2CppType;   struct Il2CppObject;

using t_domain_get                = Il2CppDomain* (*)();
using t_thread_current            = void* (*)();
using t_thread_attach             = void* (*)(Il2CppDomain*);
using t_thread_detach             = void  (*)(void*);
using t_assembly_get_image        = Il2CppImage* (*)(Il2CppAssembly*);
using t_class_get_name            = const char* (*)(Il2CppClass*);
using t_class_get_namespace       = const char* (*)(Il2CppClass*);
using t_class_get_methods         = void* (*)(Il2CppClass*, void**);
using t_class_get_fields          = void* (*)(Il2CppClass*, void**);
using t_class_get_parent          = Il2CppClass* (*)(Il2CppClass*);
using t_class_get_nested          = Il2CppClass* (*)(Il2CppClass*, void**);
using t_class_get_interfaces      = Il2CppClass* (*)(Il2CppClass*, void**);
using t_class_get_image           = Il2CppImage* (*)(Il2CppClass*);
using t_class_get_type            = Il2CppType* (*)(Il2CppClass*);
using t_class_get_flags           = uint32_t (*)(Il2CppClass*);
using t_class_enum_basetype       = Il2CppType* (*)(Il2CppClass*);
using t_class_is_enum             = bool (*)(Il2CppClass*);
using t_class_is_valuetype        = bool (*)(Il2CppClass*);
using t_class_is_interface        = bool (*)(Il2CppClass*);
using t_method_get_name           = const char* (*)(Il2CppMethod*);
using t_method_get_return         = Il2CppType* (*)(Il2CppMethod*);
using t_method_get_param_count    = uint32_t (*)(Il2CppMethod*);
using t_method_get_param          = Il2CppType* (*)(Il2CppMethod*, uint32_t);
using t_method_is_instance        = bool (*)(Il2CppMethod*);
using t_field_get_name            = const char* (*)(Il2CppField*);
using t_field_get_type            = Il2CppType* (*)(Il2CppField*);
using t_field_get_offset          = uint32_t (*)(Il2CppField*);
using t_field_get_flags           = uint32_t (*)(Il2CppField*);
using t_field_static_get_value    = void (*)(Il2CppField*, void*);
using t_type_get_name             = const char* (*)(Il2CppType*);
using t_type_is_byref             = bool (*)(Il2CppType*);
using t_type_get_object           = Il2CppObject* (*)(Il2CppType*);
using t_class_from_type           = Il2CppClass* (*)(Il2CppType*);
using t_get_all_assemblies        = Il2CppAssembly** (*)(size_t*);
using t_get_assembly_type_handle  = Il2CppClass* (*)(Il2CppAssembly*, const char*);
using t_get_type_info_from_handle = Il2CppType* (*)(Il2CppClass*);

bool init();
uintptr_t base();

Il2CppClass*  find_class (const char* ns, const char* name);
Il2CppMethod* find_method(Il2CppClass*, const char* name);
Il2CppField*  find_field (Il2CppClass*, const char* name);
void*         method_pointer(Il2CppMethod*);
uint32_t      field_offset (Il2CppClass*, const char* name);

extern t_domain_get domain_get;             extern t_thread_current thread_current;
extern t_thread_attach thread_attach;       extern t_thread_detach thread_detach;
extern t_assembly_get_image assembly_get_image;
extern t_class_get_name class_get_name;     extern t_class_get_namespace class_get_namespace;
extern t_class_get_methods class_get_methods; extern t_class_get_fields class_get_fields;
extern t_class_get_parent class_get_parent; extern t_class_get_nested class_get_nested;
extern t_class_get_interfaces class_get_interfaces; extern t_class_get_image class_get_image;
extern t_class_get_type class_get_type;     extern t_class_get_flags class_get_flags;
extern t_class_enum_basetype class_enum_basetype;
extern t_class_is_enum class_is_enum;       extern t_class_is_valuetype class_is_valuetype;
extern t_class_is_interface class_is_interface;
extern t_method_get_name method_get_name;   extern t_method_get_return method_get_return;
extern t_method_get_param_count method_get_param_count;
extern t_method_get_param method_get_param; extern t_method_is_instance method_is_instance;
extern t_field_get_name field_get_name;     extern t_field_get_type field_get_type;
extern t_field_get_offset field_get_offset; extern t_field_get_flags field_get_flags;
extern t_field_static_get_value field_static_get_value;
extern t_type_get_name type_get_name;       extern t_type_is_byref type_is_byref;
extern t_type_get_object type_get_object;   extern t_class_from_type class_from_type;
extern t_get_all_assemblies get_all_assemblies;
extern t_get_assembly_type_handle get_assembly_type_handle;
extern t_get_type_info_from_handle get_type_info_from_handle;

} // namespace il2cpp
