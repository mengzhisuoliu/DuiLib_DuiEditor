#include <check.h>
#include <stdlib.h>
#include <string.h>

#define NANOSVG_IMPLEMENTATION
#include "DuiLib/Utils/nanosvg.h"

START_TEST(test_gradient_stops_buffer_overflow)
{
    // Invariant: Parsing SVG with gradient stops must not cause buffer overflow
    const char *payloads[] = {
        // Exploit case: excessive gradient stops
        "<svg><defs><linearGradient id='g'>"
        "<stop offset='0'/><stop offset='0.1'/><stop offset='0.2'/><stop offset='0.3'/>"
        "<stop offset='0.4'/><stop offset='0.5'/><stop offset='0.6'/><stop offset='0.7'/>"
        "<stop offset='0.8'/><stop offset='0.9'/><stop offset='1.0'/><stop offset='1.1'/>"
        "<stop offset='1.2'/><stop offset='1.3'/><stop offset='1.4'/><stop offset='1.5'/>"
        "</linearGradient></defs><rect fill='url(#g)'/></svg>",
        // Boundary: exactly at typical limit
        "<svg><defs><linearGradient id='g'>"
        "<stop offset='0'/><stop offset='1'/>"
        "</linearGradient></defs><rect fill='url(#g)'/></svg>",
        // Valid: simple gradient
        "<svg><defs><linearGradient id='g'><stop offset='0.5'/></linearGradient></defs>"
        "<rect fill='url(#g)'/></svg>"
    };
    int num_payloads = sizeof(payloads) / sizeof(payloads[0]);

    for (int i = 0; i < num_payloads; i++) {
        char *input = strdup(payloads[i]);
        ck_assert_ptr_nonnull(input);
        
        NSVGimage *image = nsvgParse(input, "px", 96.0f);
        // Parser must either succeed safely or return NULL - no crash
        if (image != NULL) {
            nsvgDelete(image);
        }
        free(input);
    }
}
END_TEST

Suite *security_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Security");
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_gradient_stops_buffer_overflow);
    suite_add_tcase(s, tc_core);

    return s;
}

int main(void)
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = security_suite();
    sr = srunner_create(s);

    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}