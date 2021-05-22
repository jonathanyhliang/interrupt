#include "CppUTest/TestHarness.h"

extern "C" {
  #include "kv_store/kv_store.h"
  #include "lfs.h"
  #include "emubd/lfs_emubd.h"

  #include "defs/lfs_default_config.h"
  #include "stubs/stub_analytics.h"
  #include "fakes/fake_mutex.h"
}

TEST_GROUP(TestKvStore) {
  void setup() {
    lfs_emubd_create(&cfg, "blocks");
    lfs_format(&lfs, &cfg);
    lfs_mount(&lfs, &cfg);

    kv_store_init(&lfs);
    fake_mutex_init();
  }

  void teardown() {
    lfs_emubd_destroy(&cfg);
    lfs_unmount(&lfs);
    CHECK(fake_mutex_all_unlocked());
  }
};

TEST(TestKvStore, Test_SimpleKvStore) {
  bool success;

  const char *key = "hello";
  const char *val = "world";
  success = kv_store_write(key, (void*)val, sizeof(val));
  CHECK(success);

  char buf[16];
  uint32_t read_len;
  success = kv_store_read(key, buf, sizeof(buf), &read_len);
  CHECK(success);
  STRCMP_EQUAL(val, buf);

  success = kv_store_read(key, buf, 0, &read_len);
  CHECK_FALSE(success);
}
