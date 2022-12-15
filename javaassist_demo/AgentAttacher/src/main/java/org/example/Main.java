package org.example;

import com.sun.tools.attach.AgentInitializationException;
import com.sun.tools.attach.AgentLoadException;
import com.sun.tools.attach.AttachNotSupportedException;
import com.sun.tools.attach.VirtualMachine;

import java.io.IOException;

public class Main {
    public static void main(String[] args)
            throws AttachNotSupportedException, IOException, AgentLoadException, AgentInitializationException {
        // args[0]: pid
        // args[1]: agent jar package path
        VirtualMachine vm = VirtualMachine.attach(args[0]);
        vm.loadAgent(args[1]);
    }
}