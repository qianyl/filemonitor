//#define ITER_LOG_DISABLE
#include "basicio/file_io.hpp"
#include <file_monitor.hpp>
#include <file_keeper.hpp>
#include <gtest/gtest.h>

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <memory>
#include <map>

using namespace iter;

class FileKeeperTest : public testing::Test{
public:
    virtual void SetUp() {
        content = "girigiri eye~\ngirigiri mind~\n";
        filename = "girigiri.test";
        bool write_ret = FileWrite(filename, content);
        EXPECT_TRUE(write_ret);

        file_monitor_ptr = std::make_shared <FileMonitor> ();
        file_keeper_ptr = std::make_shared <FileKeeper <std::string>>(
                filename, FileRead, file_monitor_ptr);
        bool load_ret = file_keeper_ptr->Load();
    }

    std::string content;
    std::string filename;
    std::shared_ptr <FileMonitor> file_monitor_ptr;
    std::shared_ptr <FileKeeper <std::string>> file_keeper_ptr;
};

TEST_F(FileKeeperTest, SimpleTest) {
    auto ptr = file_keeper_ptr->Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(*ptr, content);
}

TEST_F(FileKeeperTest, ModifyTest) {
    std::string newtext = "wowuwo~ wowowo~";
    usleep(1e5);
    bool write_ret = FileWrite(filename, newtext);
    EXPECT_TRUE(write_ret);
    usleep(1e5);
    auto ptr = file_keeper_ptr->Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(*ptr, newtext);
}

void MoveWrite(const std::string& content, const std::string& filename) {
    std::string filename_tmp = filename + ".tmp";
    bool write_ret = FileWrite(filename_tmp, content);
    EXPECT_TRUE(write_ret);
    std::string cmd = "mv " + filename_tmp + " " + filename;
    system(cmd.c_str());
}

TEST_F(FileKeeperTest, MoveCoverTest) {
    std::string text_test = "GIRIGIRI_EYE\n";
    MoveWrite(text_test, filename);
    usleep(1e5);
    auto ptr = file_keeper_ptr->Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(text_test, *ptr);

    text_test = "GIRIGIRI_MINE\n";
    MoveWrite(text_test, filename);
    usleep(1e5);

    ptr = file_keeper_ptr->Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(text_test, *ptr);
}

TEST_F(FileKeeperTest, MoveSelfTest) {
    std::string cmd = "mv " + filename + " " + filename + ".tmp2";
    system(cmd.c_str());
    usleep(1e5);

    std::string text_test = "BILIBILI_EYE\n";
    bool write_ret = FileWrite(filename+".tmp2", text_test);
    EXPECT_TRUE(write_ret);

    cmd = "mv " + filename + ".tmp2 " + filename;
    system(cmd.c_str());
    usleep(1e5);

    auto ptr = file_keeper_ptr->Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(text_test, *ptr);
}

TEST_F(FileKeeperTest, DeleteTest) {
    std::string filename_test = "bilibili.test";
    std::string text_test = "GIriGIri\n";
    FileKeeper <std::string> fk(filename_test, FileRead, file_monitor_ptr);
    FileWrite(filename_test, text_test);
    usleep(1e5);

    auto ptr = fk.Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(text_test, *ptr);

    std::string cmd = "rm -rf " + filename_test;
    system(cmd.c_str());
    FileWrite(filename_test, text_test);
    usleep(1e5);

    ptr = fk.Get();
    EXPECT_TRUE(bool(ptr));
    if (ptr) EXPECT_EQ(text_test, *ptr);
}

