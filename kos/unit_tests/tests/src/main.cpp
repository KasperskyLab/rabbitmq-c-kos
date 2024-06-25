/*
 * © 2024 AO Kaspersky Lab
 * Licensed under the MIT License
 */

#include <filesystem>
#include <iomanip>
#include <iostream>
#include <cstring>

#include <sys/mount.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <inttypes.h>

#include <math.h>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

// external
#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <rabbitmq-c/ssl_socket.h>


// internal
extern "C"
{
#include "amqp_time.h"
#include "amqp_table.h"
}

#include <kos_net.h>

static void parseSuccess(
    const char *url,
    const char *user,
    const char *password,
    const char *host,
    int         port,
    const char *vhost)
{
    char *s = strdup(url);
    struct amqp_connection_info ci;

    EXPECT_FALSE(amqp_parse_url(s, &ci));

    EXPECT_STREQ(user, ci.user);
    EXPECT_STREQ(password, ci.password);
    EXPECT_STREQ(host, ci.host);
    EXPECT_EQ(port, ci.port);
    EXPECT_STREQ(vhost, ci.vhost);

    free(s);
}

static void parseFail(const char *url)
{
    char *s = strdup(url);
    struct amqp_connection_info ci;

    amqp_default_connection_info(&ci);

    EXPECT_LT(amqp_parse_url(s, &ci), 0);

    free(s);
}

TEST(LibRabbitMQTest, ParseUrl)
{
    /* From the spec */
    parseSuccess("amqp://user:pass@host:10000/vhost", "user", "pass", "host", 10000, "vhost");
    parseSuccess("amqps://user:pass@host:10000/vhost", "user", "pass", "host", 10000, "vhost");

    parseSuccess("amqp://user%61:%61pass@ho%61st:10000/v%2fhost",
                  "usera", "apass", "hoast", 10000, "v/host");

    parseSuccess("amqps://user%61:%61pass@ho%61st:10000/v%2fhost",
                  "usera", "apass", "hoast", 10000, "v/host");

    parseSuccess("amqp://", "guest", "guest", "localhost", 5672, "/");
    parseSuccess("amqps://", "guest", "guest", "localhost", 5671, "/");

    parseSuccess("amqp://:@/", "", "", "localhost", 5672, "");
    parseSuccess("amqps://:@/", "", "", "localhost", 5671, "");

    parseSuccess("amqp://user@", "user", "guest", "localhost", 5672, "/");
    parseSuccess("amqps://user@", "user", "guest", "localhost", 5671, "/");

    parseSuccess("amqp://user:pass@", "user", "pass", "localhost", 5672, "/");
    parseSuccess("amqps://user:pass@", "user", "pass", "localhost", 5671, "/");

    parseSuccess("amqp://host", "guest", "guest", "host", 5672, "/");
    parseSuccess("amqps://host", "guest", "guest", "host", 5671, "/");

    parseSuccess("amqp://:10000", "guest", "guest", "localhost", 10000, "/");
    parseSuccess("amqps://:10000", "guest", "guest", "localhost", 10000, "/");

    parseSuccess("amqp:///vhost", "guest", "guest", "localhost", 5672, "vhost");
    parseSuccess("amqps:///vhost", "guest", "guest", "localhost", 5671, "vhost");

    parseSuccess("amqp://host/", "guest", "guest", "host", 5672, "");
    parseSuccess("amqps://host/", "guest", "guest", "host", 5671, "");

    parseSuccess("amqp://host/%2f", "guest", "guest", "host", 5672, "/");
    parseSuccess("amqps://host/%2f", "guest", "guest", "host", 5671, "/");

    parseSuccess("amqp://[::1]", "guest", "guest", "::1", 5672, "/");
    parseSuccess("amqps://[::1]", "guest", "guest", "::1", 5671, "/");

    /* Various other success cases */
    parseSuccess("amqp://host:100", "guest", "guest", "host", 100, "/");
    parseSuccess("amqps://host:100", "guest", "guest", "host", 100, "/");

    parseSuccess("amqp://[::1]:100", "guest", "guest", "::1", 100, "/");
    parseSuccess("amqps://[::1]:100", "guest", "guest", "::1", 100, "/");

    parseSuccess("amqp://host/blah", "guest", "guest", "host", 5672, "blah");
    parseSuccess("amqps://host/blah", "guest", "guest", "host", 5671, "blah");

    parseSuccess("amqp://host:100/blah", "guest", "guest", "host", 100, "blah");
    parseSuccess("amqps://host:100/blah", "guest", "guest", "host", 100, "blah");

    parseSuccess("amqp://:100/blah", "guest", "guest", "localhost", 100, "blah");
    parseSuccess("amqps://:100/blah", "guest", "guest", "localhost", 100, "blah");

    parseSuccess("amqp://[::1]/blah", "guest", "guest", "::1", 5672, "blah");
    parseSuccess("amqps://[::1]/blah", "guest", "guest", "::1", 5671, "blah");

    parseSuccess("amqp://[::1]:100/blah", "guest", "guest", "::1", 100, "blah");
    parseSuccess("amqps://[::1]:100/blah", "guest", "guest", "::1", 100, "blah");

    parseSuccess("amqp://user:pass@host", "user", "pass", "host", 5672, "/");
    parseSuccess("amqps://user:pass@host", "user", "pass", "host", 5671, "/");

    parseSuccess("amqp://user:pass@host:100", "user", "pass", "host", 100, "/");
    parseSuccess("amqps://user:pass@host:100", "user", "pass", "host", 100, "/");

    parseSuccess("amqp://user:pass@:100", "user", "pass", "localhost", 100, "/");
    parseSuccess("amqps://user:pass@:100", "user", "pass", "localhost", 100, "/");

    parseSuccess("amqp://user:pass@[::1]", "user", "pass", "::1", 5672, "/");
    parseSuccess("amqps://user:pass@[::1]", "user", "pass", "::1", 5671, "/");

    parseSuccess("amqp://user:pass@[::1]:100", "user", "pass", "::1", 100, "/");
    parseSuccess("amqps://user:pass@[::1]:100", "user", "pass", "::1", 100, "/");

    /* Various failure cases */
    parseFail("http://www.rabbitmq.com");

    parseFail("amqp://foo:bar:baz");
    parseFail("amqps://foo:bar:baz");

    parseFail("amqp://foo[::1]");
    parseFail("amqps://foo[::1]");

    parseFail("amqp://foo[::1]");
    parseFail("amqps://foo[::1]");

    parseFail("amqp://foo:[::1]");
    parseFail("amqps://foo:[::1]");

    parseFail("amqp://[::1]foo");
    parseFail("amqps://[::1]foo");

    parseFail("amqp://foo:1000xyz");
    parseFail("amqps://foo:1000xyz");

    parseFail("amqp://foo:1000000");
    parseFail("amqps://foo:1000000");

    parseFail("amqp://foo/bar/baz");
    parseFail("amqps://foo/bar/baz");

    parseFail("amqp://foo%1");
    parseFail("amqps://foo%1");

    parseFail("amqp://foo%1x");
    parseFail("amqps://foo%1x");

    parseFail("amqp://foo%xy");
    parseFail("amqps://foo%xy");
}

////////////////////////// Tables ////////////////////////////////////////////////////////////////

const char* expected = "AAAAAAAAAA\n" \
                 "F\n" \
                 "  longstr ->\n" \
                 "    S Here is a long string\n" \
                 "  signedint ->\n" \
                 "    I 12345\n" \
                 "  decimal ->\n" \
                 "    D 3:::123456\n" \
                 "  timestamp ->\n"
                 "    T 109876543209876\n"
                 "  table ->\n"
                 "    F\n"
                 "      one ->\n"
                 "        I 54321\n"
                 "      two ->\n"
                 "        S A long string\n"
                 "  byte ->\n"
                 "    b -1\n"
                 "  long ->\n"
                 "    l 1234567890\n"
                 "  short ->\n"
                 "    s 655\n"
                 "  bool ->\n"
                 "    t true\n"
                 "  binary ->\n"
                 "    x 612062696e61727920737472696e67\n"
                 "  void ->\n"
                 "    V\n"
                 "  array ->\n"
                 "    A\n"
                 "      I 54321\n"
                 "      S A long string\n"
                 "  float ->\n"
                 "    f 3.14159\n"
                 "  double ->\n"
                 "    d 3.14159\n"
                 "BBBBBBBBBB\n"
                 "F\n"
                 "  longstr ->\n"
                 "    S Here is a long string\n"
                 "  signedint ->\n"
                 "    I 12345\n"
                 "  decimal ->\n"
                 "    D 3:::123456\n"
                 "  timestamp ->\n"
                 "    T 109876543209876\n"
                 "  table ->\n"
                 "    F\n"
                 "      one ->\n"
                 "        I 54321\n"
                 "      two ->\n"
                 "        S A long string\n"
                 "  byte ->\n"
                 "    b -1\n"
                 "  long ->\n"
                 "    l 1234567890\n"
                 "  short ->\n"
                 "    s 655\n"
                 "  bool ->\n"
                 "    t true\n"
                 "  binary ->\n"
                 "    x 612062696e61727920737472696e67\n"
                 "  void ->\n"
                 "    V\n"
                 "  array ->\n"
                 "    A\n"
                 "      I 54321\n"
                 "      S A long string\n"
                 "  float ->\n"
                 "    f 3.14159\n"
                 "  double ->\n"
                 "    d 3.14159\n"
                 "----------\n"
                 "F\n"
                 "  aardvark ->\n"
                 "    S first\n"
                 "  beta ->\n"
                 "    S second\n"
                 "  decimal ->\n"
                 "    D 2:::1234\n"
                 "  middle ->\n"
                 "    S third\n"
                 "  number ->\n"
                 "    I 1234\n"
                 "  time ->\n"
                 "    T 1234123412341234\n"
                 "  wombat ->\n"
                 "    S fourth\n"
                 "  zebra ->\n"
                 "    S last\n"
                 "";


static void dumpIndent(int indent, char *out)
{
    int i;

    for (i = 0; i < indent; i++)
    {
        strcat(out, " ");
    }
}

static void dumpValue(int indent, amqp_field_value_t v, char *out)
{
    int  i;
    char tmp_out[1024] = {0};

    dumpIndent(indent, out);
    strncat(out, (const char *)&(v.kind), 1);

    switch (v.kind)
    {
        case AMQP_FIELD_KIND_BOOLEAN:
            sprintf(tmp_out, v.value.boolean ? " true\n" : " false\n");
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_I8:
            sprintf(tmp_out, " %" PRId8 "\n", v.value.i8);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_U8:
            sprintf(tmp_out, " %" PRIu8 "\n", v.value.u8);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_I16:
            sprintf(tmp_out, " %" PRId16 "\n", v.value.i16);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_U16:
            sprintf(tmp_out, " %" PRIu16 "\n", v.value.u16);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_I32:
            sprintf(tmp_out, " %" PRId32 "\n", v.value.i32);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_U32:
            sprintf(tmp_out, " %" PRIu32 "\n", v.value.u32);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_I64:
            sprintf(tmp_out, " %" PRId64 "\n", v.value.i64);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_F32:
            sprintf(tmp_out, " %g\n", (double)v.value.f32);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_F64:
            sprintf(tmp_out, " %g\n", v.value.f64);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_DECIMAL:
            sprintf(tmp_out, " %u:::%u\n", v.value.decimal.decimals, v.value.decimal.value);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_UTF8:
            sprintf(tmp_out, " %.*s\n", (int)v.value.bytes.len, (char *)v.value.bytes.bytes);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_BYTES:
            strcat(out, " ");
            for (i = 0; i < (int)v.value.bytes.len; i++)
            {
                sprintf(tmp_out, "%02x", ((char *)v.value.bytes.bytes)[i]);
                strcat(out, tmp_out);
            }

            strcat(out, "\n");
            break;

        case AMQP_FIELD_KIND_ARRAY:
            strcat(out, "\n");
            for (i = 0; i < v.value.array.num_entries; i++)
            {
                dumpValue(indent + 2, v.value.array.entries[i], out);
            }

            break;

        case AMQP_FIELD_KIND_TIMESTAMP:
            sprintf(tmp_out, " %" PRIu64 "\n", v.value.u64);
            strcat(out, tmp_out);
            break;

        case AMQP_FIELD_KIND_TABLE:
            strcat(out, "\n");
            for (i = 0; i < v.value.table.num_entries; i++)
            {
                dumpIndent(indent + 2, out);
                sprintf(
                    tmp_out,
                    "%.*s ->\n",
                    (int)v.value.table.entries[i].key.len,
                    (char *)v.value.table.entries[i].key.bytes);
                strcat(out, tmp_out);
                dumpValue(indent + 4, v.value.table.entries[i].value, out);
            }

            break;

        case AMQP_FIELD_KIND_VOID:
            strcat(out, "\n");
            break;

        default:
            sprintf(tmp_out, "???\n");
            strcat(out, tmp_out);
            break;
    }
}

static void testDumpValue(char *out)
{
    amqp_table_entry_t entries[8];
    amqp_table_t       table;
    amqp_field_value_t val;

    entries[0].key               = amqp_cstring_bytes("zebra");
    entries[0].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[0].value.value.bytes = amqp_cstring_bytes("last");

    entries[1].key               = amqp_cstring_bytes("aardvark");
    entries[1].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[1].value.value.bytes = amqp_cstring_bytes("first");

    entries[2].key               = amqp_cstring_bytes("middle");
    entries[2].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[2].value.value.bytes = amqp_cstring_bytes("third");

    entries[3].key             = amqp_cstring_bytes("number");
    entries[3].value.kind      = AMQP_FIELD_KIND_I32;
    entries[3].value.value.i32 = 1234;

    entries[4].key                          = amqp_cstring_bytes("decimal");
    entries[4].value.kind                   = AMQP_FIELD_KIND_DECIMAL;
    entries[4].value.value.decimal.decimals = 2;
    entries[4].value.value.decimal.value    = 1234;

    entries[5].key             = amqp_cstring_bytes("time");
    entries[5].value.kind      = AMQP_FIELD_KIND_TIMESTAMP;
    entries[5].value.value.u64 = 1234123412341234;

    entries[6].key               = amqp_cstring_bytes("beta");
    entries[6].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[6].value.value.bytes = amqp_cstring_bytes("second");

    entries[7].key               = amqp_cstring_bytes("wombat");
    entries[7].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[7].value.value.bytes = amqp_cstring_bytes("fourth");

    table.num_entries  = 8;
    table.entries      = entries;

    qsort(table.entries, table.num_entries, sizeof(amqp_table_entry_t), &amqp_table_entry_cmp);

    val.kind        = AMQP_FIELD_KIND_TABLE;
    val.value.table = table;

    dumpValue(0, val, out);
}

static uint8_t preEncodedTable[] = {
    0x00, 0x00, 0x00, 0xff, 0x07, 0x6c, 0x6f, 0x6e, 0x67, 0x73, 0x74, 0x72, 0x53, 0x00, 0x00, 0x00,
    0x15, 0x48, 0x65, 0x72, 0x65, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x6c, 0x6f, 0x6e, 0x67, 0x20,
    0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x09, 0x73, 0x69, 0x67, 0x6e, 0x65, 0x64, 0x69, 0x6e, 0x74,
    0x49, 0x00, 0x00, 0x30, 0x39, 0x07, 0x64, 0x65, 0x63, 0x69, 0x6d, 0x61, 0x6c, 0x44, 0x03, 0x00,
    0x01, 0xe2, 0x40, 0x09, 0x74, 0x69, 0x6d, 0x65, 0x73, 0x74, 0x61, 0x6d, 0x70, 0x54, 0x00, 0x00,
    0x63, 0xee, 0xa0, 0x53, 0xc1, 0x94, 0x05, 0x74, 0x61, 0x62, 0x6c, 0x65, 0x46, 0x00, 0x00, 0x00,
    0x1f, 0x03, 0x6f, 0x6e, 0x65, 0x49, 0x00, 0x00, 0xd4, 0x31, 0x03, 0x74, 0x77, 0x6f, 0x53, 0x00,
    0x00, 0x00, 0x0d, 0x41, 0x20, 0x6c, 0x6f, 0x6e, 0x67, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67,
    0x04, 0x62, 0x79, 0x74, 0x65, 0x62, 0xff, 0x04, 0x6c, 0x6f, 0x6e, 0x67, 0x6c, 0x00, 0x00, 0x00,
    0x00, 0x49, 0x96, 0x02, 0xd2, 0x05, 0x73, 0x68, 0x6f, 0x72, 0x74, 0x73, 0x02, 0x8f, 0x04, 0x62,
    0x6f, 0x6f, 0x6c, 0x74, 0x01, 0x06, 0x62, 0x69, 0x6e, 0x61, 0x72, 0x79, 0x78, 0x00, 0x00, 0x00,
    0x0f, 0x61, 0x20, 0x62, 0x69, 0x6e, 0x61, 0x72, 0x79, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67,
    0x04, 0x76, 0x6f, 0x69, 0x64, 0x56, 0x05, 0x61, 0x72, 0x72, 0x61, 0x79, 0x41, 0x00, 0x00, 0x00,
    0x17, 0x49, 0x00, 0x00, 0xd4, 0x31, 0x53, 0x00, 0x00, 0x00, 0x0d, 0x41, 0x20, 0x6c, 0x6f, 0x6e,
    0x67, 0x20, 0x73, 0x74, 0x72, 0x69, 0x6e, 0x67, 0x05, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x66, 0x40,
    0x49, 0x0f, 0xdb, 0x06, 0x64, 0x6f, 0x75, 0x62, 0x6c, 0x65, 0x64, 0x40, 0x09, 0x21, 0xfb, 0x54,
    0x44, 0x2d, 0x18};

static void testTableCodec(char *out)
{
    amqp_pool_t pool;
    int         result;
    char        tmpOut[1024] = {0};

    amqp_table_entry_t innerEntries[2];
    amqp_table_t       innerTable;

    amqp_field_value_t innerValues[2];
    amqp_array_t       innerArray;

    amqp_table_entry_t entries[14];
    amqp_table_t       table;

    innerEntries[0].key             = amqp_cstring_bytes("one");
    innerEntries[0].value.kind      = AMQP_FIELD_KIND_I32;
    innerEntries[0].value.value.i32 = 54321;

    innerEntries[1].key               = amqp_cstring_bytes("two");
    innerEntries[1].value.kind        = AMQP_FIELD_KIND_UTF8;
    innerEntries[1].value.value.bytes = amqp_cstring_bytes("A long string");

    innerTable.num_entries = 2;
    innerTable.entries     = innerEntries;

    innerValues[0].kind      = AMQP_FIELD_KIND_I32;
    innerValues[0].value.i32 = 54321;

    innerValues[1].kind        = AMQP_FIELD_KIND_UTF8;
    innerValues[1].value.bytes = amqp_cstring_bytes("A long string");

    innerArray.num_entries = 2;
    innerArray.entries     = innerValues;

    entries[0].key               = amqp_cstring_bytes("longstr");
    entries[0].value.kind        = AMQP_FIELD_KIND_UTF8;
    entries[0].value.value.bytes = amqp_cstring_bytes("Here is a long string");

    entries[1].key             = amqp_cstring_bytes("signedint");
    entries[1].value.kind      = AMQP_FIELD_KIND_I32;
    entries[1].value.value.i32 = 12345;

    entries[2].key                          = amqp_cstring_bytes("decimal");
    entries[2].value.kind                   = AMQP_FIELD_KIND_DECIMAL;
    entries[2].value.value.decimal.decimals = 3;
    entries[2].value.value.decimal.value    = 123456;

    entries[3].key             = amqp_cstring_bytes("timestamp");
    entries[3].value.kind      = AMQP_FIELD_KIND_TIMESTAMP;
    entries[3].value.value.u64 = 109876543209876;

    entries[4].key               = amqp_cstring_bytes("table");
    entries[4].value.kind        = AMQP_FIELD_KIND_TABLE;
    entries[4].value.value.table = innerTable;

    entries[5].key            = amqp_cstring_bytes("byte");
    entries[5].value.kind     = AMQP_FIELD_KIND_I8;
    entries[5].value.value.i8 = (int8_t)-1;

    entries[6].key             = amqp_cstring_bytes("long");
    entries[6].value.kind      = AMQP_FIELD_KIND_I64;
    entries[6].value.value.i64 = 1234567890;

    entries[7].key             = amqp_cstring_bytes("short");
    entries[7].value.kind      = AMQP_FIELD_KIND_I16;
    entries[7].value.value.i16 = 655;

    entries[8].key                 = amqp_cstring_bytes("bool");
    entries[8].value.kind          = AMQP_FIELD_KIND_BOOLEAN;
    entries[8].value.value.boolean = 1;

    entries[9].key               = amqp_cstring_bytes("binary");
    entries[9].value.kind        = AMQP_FIELD_KIND_BYTES;
    entries[9].value.value.bytes = amqp_cstring_bytes("a binary string");

    entries[10].key        = amqp_cstring_bytes("void");
    entries[10].value.kind = AMQP_FIELD_KIND_VOID;

    entries[11].key               = amqp_cstring_bytes("array");
    entries[11].value.kind        = AMQP_FIELD_KIND_ARRAY;
    entries[11].value.value.array = innerArray;

    entries[12].key             = amqp_cstring_bytes("float");
    entries[12].value.kind      = AMQP_FIELD_KIND_F32;
    entries[12].value.value.f32 = (float)M_PI;

    entries[13].key             = amqp_cstring_bytes("double");
    entries[13].value.kind      = AMQP_FIELD_KIND_F64;
    entries[13].value.value.f64 = M_PI;

    table.num_entries = 14;
    table.entries     = entries;

    sprintf(tmpOut, "AAAAAAAAAA\n");
    strcat(out, tmpOut);

    {
        amqp_field_value_t val;
        val.kind        = AMQP_FIELD_KIND_TABLE;
        val.value.table = table;
        dumpValue(0, val, out);
    }

    init_amqp_pool(&pool, 4096);

    {
        amqp_table_t decoded;
        size_t       decodingOffset = 0;
        amqp_bytes_t decodingBytes;
        decodingBytes.len   = sizeof(preEncodedTable);
        decodingBytes.bytes = preEncodedTable;

        result = amqp_decode_table(decodingBytes, &pool, &decoded, &decodingOffset);

        EXPECT_GE(result, 0);

        sprintf(tmpOut, "BBBBBBBBBB\n");
        strcat(out, tmpOut);

        {
            amqp_field_value_t val;
            val.kind        = AMQP_FIELD_KIND_TABLE;
            val.value.table = decoded;

            dumpValue(0, val, out);
        }
    }

    {
        uint8_t      encodingBuffer[4096];
        amqp_bytes_t encodingResult;
        size_t       offset = 0;

        memset(encodingBuffer, 0, sizeof(encodingBuffer));
        encodingResult.len   = sizeof(encodingBuffer);
        encodingResult.bytes = &encodingBuffer[0];

        result = amqp_encode_table(encodingResult, &table, &offset);

        EXPECT_GE(result, 0);

        EXPECT_EQ(offset, sizeof(preEncodedTable));

        result = memcmp(preEncodedTable, encodingBuffer, offset);

        EXPECT_EQ(result, 0);
    }

    empty_amqp_pool(&pool);
}

#define CHUNK_SIZE 4096

static int compareFiles(char *f1_in, char *f2_in)
{
    int res;

    for (;;)
    {
        size_t f1_got = strlen(f1_in);
        size_t f2_got = strlen(f2_in);

        res = memcmp(f1_in, f2_in, f1_got < f2_got ? f1_got : f2_got);

        if (res)
        {
            break;
        }

        if (f1_got < CHUNK_SIZE || f2_got < CHUNK_SIZE)
        {
            if (f1_got != f2_got)
            {
                res = (f1_got < f2_got ? -1 : 1);
            }
            break;
        }
    }

    return res;
}

TEST(LibRabbitMQTest, Tables)
{
    char out[CHUNK_SIZE]         = {0};
    char tmpOut[CHUNK_SIZE / 4] = {0};

    testTableCodec(out);
    sprintf(tmpOut, "----------\n");
    strcat(out, tmpOut);
    testDumpValue(out);

    EXPECT_EQ(compareFiles(const_cast<char *>(expected), out), 0);
}

static void checkErrorStrings(amqp_status_enum start, amqp_status_enum end)
{
    int i;
    for (i = start; i > end; --i)
    {
        EXPECT_STRNE(amqp_error_string2(i), "(unknown error)");
    }
}

TEST(LibRabbitMQTest, StatusEnum)
{
    checkErrorStrings(AMQP_STATUS_OK, _AMQP_STATUS_NEXT_VALUE);
    checkErrorStrings(AMQP_STATUS_TCP_ERROR, _AMQP_STATUS_TCP_NEXT_VALUE);
    checkErrorStrings(AMQP_STATUS_SSL_ERROR, _AMQP_STATUS_SSL_NEXT_VALUE);
}

static void parseSuccess(amqp_bytes_t mechanisms, amqp_sasl_method_enum method)
{
    EXPECT_TRUE(sasl_mechanism_in_list(mechanisms, method));
}

static void parseFail(amqp_bytes_t mechanisms, amqp_sasl_method_enum method)
{
    EXPECT_FALSE(sasl_mechanism_in_list(mechanisms, method));
}

TEST(LibRabbitMQTest, SaslMechanism)
{
    parseSuccess(amqp_cstring_bytes("DIGEST-MD5 CRAM-MD5 LOGIN PLAIN"), AMQP_SASL_METHOD_PLAIN);
    parseFail(amqp_cstring_bytes("DIGEST-MD5 CRAM-MD5 LOGIN PLAIN"), AMQP_SASL_METHOD_EXTERNAL);
    parseSuccess(amqp_cstring_bytes("DIGEST-MD5 CRAM-MD5 EXTERNAL"), AMQP_SASL_METHOD_EXTERNAL);
    parseFail(amqp_cstring_bytes("DIGEST-MD5 CRAM-MD5 EXTERNAL"), AMQP_SASL_METHOD_PLAIN);
}

static int compareFieldValue(amqp_field_value_t result, amqp_field_value_t expect);
static int compareAmqpTable(amqp_table_t *result, amqp_table_t *expect);

static int compareBytes(amqp_bytes_t l, amqp_bytes_t r)
{
    if (l.len == r.len && (l.bytes == r.bytes || 0 == memcmp(l.bytes, r.bytes, l.len)))
    {
        return 1;
    }
    return 0;
}

static int compareAmqpTableEntry(amqp_table_entry_t result, amqp_table_entry_t expect)
{
    if (!compareBytes(result.key, expect.key))
    {
        return 0;
    }
    return compareFieldValue(result.value, expect.value);
}

static int compareFieldValue(amqp_field_value_t result, amqp_field_value_t expect)
{
    if (result.kind != expect.kind)
    {
        return 0;
    }

    switch (result.kind)
    {
        case AMQP_FIELD_KIND_BOOLEAN:
            return result.value.boolean == expect.value.boolean;
        case AMQP_FIELD_KIND_I8:
            return result.value.i8 == expect.value.i8;
        case AMQP_FIELD_KIND_U8:
            return result.value.u8 == expect.value.u8;
        case AMQP_FIELD_KIND_I16:
            return result.value.i16 == expect.value.i16;
        case AMQP_FIELD_KIND_U16:
            return result.value.u16 == expect.value.u16;
        case AMQP_FIELD_KIND_I32:
            return result.value.i32 == expect.value.i32;
        case AMQP_FIELD_KIND_U32:
            return result.value.u32 == expect.value.u32;
        case AMQP_FIELD_KIND_I64:
            return result.value.i64 == expect.value.i64;
        case AMQP_FIELD_KIND_U64:
        case AMQP_FIELD_KIND_TIMESTAMP:
            return result.value.u64 == expect.value.u64;
        case AMQP_FIELD_KIND_F32:
            return result.value.f32 == expect.value.f32;
        case AMQP_FIELD_KIND_F64:
            return result.value.f64 == expect.value.f64;
        case AMQP_FIELD_KIND_DECIMAL:
            return !memcmp(
                &result.value.decimal, &expect.value.decimal, sizeof(expect.value.decimal));
        case AMQP_FIELD_KIND_UTF8:
        case AMQP_FIELD_KIND_BYTES:
            return compareBytes(result.value.bytes, expect.value.bytes);
        case AMQP_FIELD_KIND_ARRAY:
        {
            int i;
            if (result.value.array.num_entries != expect.value.array.num_entries)
            {
                return 0;
            }
            for (i = 0; i < result.value.array.num_entries; ++i)
            {
                if (!compareFieldValue(
                        result.value.array.entries[i], expect.value.array.entries[i]))
                {
                    return 0;
                }
            }
            return 1;
        }
        case AMQP_FIELD_KIND_TABLE:
            return compareAmqpTable(&result.value.table, &expect.value.table);
        case AMQP_FIELD_KIND_VOID:
            return 1;
    }
    return 1;
}

static int compareAmqpTable(amqp_table_t *result, amqp_table_t *expect)
{
    int i;

    if (result->num_entries != expect->num_entries)
    {
        return 0;
    }

    for (i = 0; i < expect->num_entries; ++i)
    {
        if (!compareAmqpTableEntry(expect->entries[i], result->entries[i]))
        {
            return 0;
        }
    }
    return 1;
}

static void testMergeCapabilities(amqp_table_t *base, amqp_table_t *add, amqp_table_t *expect)
{
    amqp_pool_t  pool;
    amqp_table_t result;

    init_amqp_pool(&pool, 4096);

    EXPECT_EQ(amqp_merge_capabilities(base, add, &result, &pool), AMQP_STATUS_OK);

    EXPECT_TRUE(compareAmqpTable(&result, expect));

    empty_amqp_pool(&pool);
}

TEST(LibRabbitMQTest, MergeCapabilities)
{
    amqp_table_t subBase;
    amqp_table_t subAdd;
    amqp_table_t subExpect;
    amqp_table_t base;
    amqp_table_t add;
    amqp_table_t expect;

    amqp_table_entry_t subBaseEntries[1];
    amqp_table_entry_t subAddEntries[2];
    amqp_table_entry_t subExpectEntries[2];

    amqp_table_entry_t baseEntries[3];
    amqp_table_entry_t addEntries[3];
    amqp_table_entry_t expectEntries[4];

    subBaseEntries[0]   = amqp_table_construct_utf8_entry("foo", "bar");
    subBase.num_entries = sizeof(subBaseEntries) / sizeof(amqp_table_entry_t);
    subBase.entries     = subBaseEntries;

    subAddEntries[0]   = amqp_table_construct_utf8_entry("something", "else");
    subAddEntries[1]   = amqp_table_construct_utf8_entry("foo", "baz");
    subAdd.num_entries = sizeof(subAddEntries) / sizeof(amqp_table_entry_t);
    subAdd.entries     = subAddEntries;

    subExpectEntries[0]   = amqp_table_construct_utf8_entry("foo", "baz");
    subExpectEntries[1]   = amqp_table_construct_utf8_entry("something", "else");
    subExpect.num_entries = sizeof(subExpectEntries) / sizeof(amqp_table_entry_t);
    subExpect.entries     = subExpectEntries;

    baseEntries[0]   = amqp_table_construct_utf8_entry("product", "1.0");
    baseEntries[1]   = amqp_table_construct_utf8_entry("nooverride", "yeah");
    baseEntries[2]   = amqp_table_construct_table_entry("props", &subBase);
    base.num_entries = sizeof(baseEntries) / sizeof(amqp_table_entry_t);
    base.entries     = baseEntries;

    addEntries[0]   = amqp_table_construct_bool_entry("bool_entry", 1);
    addEntries[1]   = amqp_table_construct_utf8_entry("product", "2.0");
    addEntries[2]   = amqp_table_construct_table_entry("props", &subAdd);
    add.num_entries = sizeof(addEntries) / sizeof(amqp_table_entry_t);
    add.entries     = addEntries;

    expectEntries[0]   = amqp_table_construct_utf8_entry("product", "2.0"),
    expectEntries[1]   = amqp_table_construct_utf8_entry("nooverride", "yeah"),
    expectEntries[2]   = amqp_table_construct_table_entry("props", &subExpect);
    expectEntries[3]   = amqp_table_construct_bool_entry("bool_entry", 1);
    expect.num_entries = sizeof(expectEntries) / sizeof(amqp_table_entry_t);
    expect.entries     = expectEntries;

    testMergeCapabilities(&base, &add, &expect);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleMock(&argc, argv);
    ::testing::InitGoogleTest(&argc, argv);

    if (!wait_for_network())
    {
        std::cerr << "Network up failed\n";
        return EXIT_FAILURE;
    }

    return RUN_ALL_TESTS();
}
