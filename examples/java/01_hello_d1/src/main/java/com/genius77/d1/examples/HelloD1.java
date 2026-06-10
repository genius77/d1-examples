// D1 Java Hello D1 Example
// Demonstrates D1 SDK basic lifecycle:
//   1. Print version
//   2. Initialize D1 (default config)
//   3. Set default request handler
//   4. Start D1 runtime
//   5. Block until exit signal (WaitStop built-in signal handling + auto stop)
//
// D1 dynamic library dependency: >= v1.2.0
//
// Prerequisite: Place D1 dynamic library into deps/ directory.

package com.genius77.d1.examples;

import com.genius77.d1.D1;

public class HelloD1 {

    public static void main(String[] args) {
        System.out.println("=== D1 Java Hello D1 ===");

        try {
            // 1. Get and print D1 library version
            String version = D1.version();
            System.out.println("D1 version: " + version);

            // 2. Initialize D1 with default config
            //    Pass null to read default config from deps/ directory
            D1.init(null);
            System.out.println("D1::init() succeeded");

            // 3. Set default request handler
            //    All unmatched requests will be routed to this handler
            D1.setDefaultHandler((taskId, msgName, payload) -> {
                System.out.println("[Handler] taskId=" + taskId +
                        ", msgName=" + msgName + ", payload=" + payload);

                // Build response
                String response = "{\"status\":\"ok\",\"msg\":\"hello from Java handler\",\"taskId\":" + taskId + "}";
                System.out.println("Reply: " + response);

                // [0]=response payload, [1]=error (null means none), [2]=return code (0=success)
                return new Object[]{response, null, 0};
            });
            System.out.println("Default handler registered");

            // 4. Start D1 runtime
            D1.start();
            System.out.println("D1::start() succeeded");

            // 5. Block until exit signal, auto stop
            //    D1::waitStop() internally listens for SIGINT/SIGTERM (Ctrl+C),
            //    and automatically calls D1::stop() for graceful shutdown
            System.out.println("D1 running, press Ctrl+C to exit...");
            D1.waitStop();
            System.out.println("D1 fully stopped, exiting.");

        } catch (D1.D1Exception e) {
            System.out.println("D1 error: " + e.getMessage());
            System.out.println("ErrorCode: " + e.getErrorCode() + ", Function: " + e.getFunctionName());
            System.exit(1);
        } catch (Exception e) {
            System.out.println("Unexpected error: " + e.getMessage());
            e.printStackTrace(System.out);
            System.exit(1);
        }
    }
}