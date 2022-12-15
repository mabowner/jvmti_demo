#include <iostream>
#include <string.h>
#include <jvmti.h>

static std::string decode_class_signature(char* class_sig)
{
    if (class_sig == nullptr) {
        return "";
    }

    switch (class_sig[0]) {
        case 'B': return "byte";
        case 'C': return "char";
        case 'S': return "short";
        case 'I': return "int";
        case 'J': return "long";
        case 'F': return "float";
        case 'D': return "double";
        case 'Z': return "boolean";
        case '[': return decode_class_signature(class_sig + 1) + "[]";
    }

    // Strip 'L' and ';'
    class_sig++;
    class_sig[strlen(class_sig) - 1] = 0;

    // Replace '/' with '.'
    for (char* c = class_sig; *c; c++) {
        if (*c == '/') *c = '.';
    }

    return class_sig;
}

std::string getMethodName(jvmtiEnv* jvmti, jmethodID mid)
{
    jclass method_class = nullptr;
    char* class_sig = nullptr;
    char* method_name = nullptr;
    std::string result = "unknown";

    if (jvmti->GetMethodDeclaringClass(mid, &method_class)== 0 &&
    jvmti->GetClassSignature(method_class, &class_sig, nullptr) == 0 &&
    jvmti->GetMethodName(mid, &method_name, nullptr, nullptr)== 0) {
        result.assign(decode_class_signature(class_sig) + "." + method_name);
    }

    jvmti->Deallocate((unsigned char*) method_name);
    jvmti->Deallocate((unsigned char*) class_sig);
    return result;
}

void JNICALL ExceptionCallback(jvmtiEnv* jvmti, JNIEnv* env, jthread thread,
                               jmethodID method, jlocation location, jobject exception,
                               jmethodID catch_method, jlocation catch_location)
{
    if (jvmti == nullptr || env == nullptr) {
        return;
    }

    char* exception_class_name = nullptr;
    jclass exception_class = env->GetObjectClass(exception);
    jvmti->GetClassSignature(exception_class, &exception_class_name, nullptr);
    std::cout << "method: " << getMethodName(jvmti, method) << ", catch method: " << getMethodName(jvmti, catch_method) << ", exception class: " << exception_class_name << std::endl;

    jvmti->Deallocate((unsigned char*) exception_class_name);
}

JNIEXPORT jint JNICALL Agent_OnLoad(JavaVM* vm, char* options, void* reserved)
{
    jvmtiEnv* jvmti = nullptr;
    vm->GetEnv((void**)&jvmti, JVMTI_VERSION_1_0);

    jvmtiCapabilities capabilities = {0};
    capabilities.can_generate_exception_events = 1;
    capabilities.can_get_line_numbers = 1;
    jvmti->AddCapabilities(&capabilities);

    jvmtiEventCallbacks callbacks {0};
    callbacks.Exception = ExceptionCallback;
    jvmti->SetEventCallbacks(&callbacks, sizeof(callbacks));
    jvmti->SetEventNotificationMode(JVMTI_ENABLE, JVMTI_EVENT_EXCEPTION, nullptr);

    return 0;
}
