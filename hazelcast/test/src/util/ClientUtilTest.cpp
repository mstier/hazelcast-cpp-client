/*
 * Copyright (c) 2008-2017, Hazelcast, Inc. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
//
// Created by sancar koyunlu on 22/08/14.
//

#include "hazelcast/util/Util.h"
#include "hazelcast/util/Future.h"
#include "hazelcast/util/Thread.h"
#include "hazelcast/util/CountDownLatch.h"

#include <ctime>
#include <errno.h>
#include <gtest/gtest.h>

namespace hazelcast {
    namespace client {
        namespace test {
            class ClientUtilTest : public ::testing::Test {
            protected:
                static void wakeTheConditionUp(util::ThreadArgs& args) {
                    util::Mutex *mutex = (util::Mutex *)args.arg0;
                    util::ConditionVariable *cv = (util::ConditionVariable *)args.arg1;
                    int wakeUpTime = *(int *)args.arg2;
                    util::sleep(wakeUpTime);

                    util::LockGuard lockGuard(*mutex);
                    cv->notify();
                }

                static void setValueToFuture(util::ThreadArgs& args) {
                    util::Future<int> *future = (util::Future<int> *)args.arg0;
                    int value = *(int *)args.arg1;
                    int wakeUpTime = *(int *)args.arg2;
                    util::sleep(wakeUpTime);
                    future->set_value(value);
                }

                static void setExceptionToFuture(util::ThreadArgs& args) {
                    util::Future<int> *future = (util::Future<int> *)args.arg0;
                    int wakeUpTime = *(int *)args.arg1;
                    util::sleep(wakeUpTime);
                    std::auto_ptr<client::exception::IException> exception(new exception::IException("exceptionName", "details"));
                    future->set_exception(exception);
                }

                static void cancelJoinFromRunningThread(util::ThreadArgs& args) {
                    util::Thread *currentThread = args.currentThread;
                    util::CountDownLatch *latch = (util::CountDownLatch *) args.arg0;
                    currentThread->cancel();
                    ASSERT_FALSE(currentThread->join());
                    latch->countDown();
                }

                static void notifyExitingThread(util::ThreadArgs& args) {
                    util::CountDownLatch *latch = (util::CountDownLatch *) args.arg0;
                    latch->countDown();
                }
            };

            TEST_F(ClientUtilTest, testConditionWaitTimeout) {
                util::Mutex mutex;
                util::ConditionVariable conditionVariable;
                int wakeUpTime = 3;
                util::Thread thread(wakeTheConditionUp, &mutex, &conditionVariable, &wakeUpTime);
                int waitSeconds = 30;
                {
                    util::LockGuard lockGuard(mutex);
                    time_t beg = time(NULL);
                    time_t end = 0;
                    bool wokenUpByInterruption = conditionVariable.waitFor(mutex, waitSeconds * 1000);
                    if (wokenUpByInterruption) {
                        end = time(NULL);
                    }
                    ASSERT_NEAR((double)(end-beg), (double)wakeUpTime, 1);
                }

            }

            TEST_F(ClientUtilTest, testConditionVariableForEINVAL) {
                util::Mutex mutex;
                util::ConditionVariable conditionVariable;
                int wakeUpTime = 1;
                util::Thread thread(wakeTheConditionUp, &mutex, &conditionVariable, &wakeUpTime);
                {
                    util::LockGuard lockGuard(mutex);
                    // the following call should not fail with assertion for EINVAL
                    conditionVariable.waitFor(mutex, 19999);
                }
            }

            TEST_F (ClientUtilTest, testFutureWaitTimeout) {
                util::Future<int> future;
                int waitSeconds = 3;
                time_t beg = time(NULL);
                ASSERT_FALSE(future.waitFor(waitSeconds * 1000));
                ASSERT_NEAR((double)(time(NULL) - beg), (double)waitSeconds, 1);
            }

            TEST_F (ClientUtilTest, testFutureSetValue) {
                util::Future<int> future;
                int waitSeconds = 3;
                int expectedValue = 2;
                future.set_value(expectedValue);
                ASSERT_TRUE(future.waitFor(waitSeconds * 1000));
                int value = future.get();
                ASSERT_EQ(expectedValue, value);
            }

            TEST_F (ClientUtilTest, testFutureSetException) {
                util::Future<int> future;

                std::auto_ptr<client::exception::IException> exception(new exception::IException("exceptionName", "details"));
                future.set_exception(exception);

                ASSERT_THROW(future.get(), exception::IException);
            }


            TEST_F (ClientUtilTest, testFutureSetValue_afterSomeTime) {
                util::Future<int> future;
                int waitSeconds = 30;
                int wakeUpTime = 3;
                int expectedValue = 2;
                util::Thread thread(ClientUtilTest::setValueToFuture, &future, &expectedValue, &wakeUpTime);
                ASSERT_TRUE(future.waitFor(waitSeconds * 1000));
                int value = future.get();
                ASSERT_EQ(expectedValue, value);

            }

            TEST_F (ClientUtilTest, testFutureSetException_afterSomeTime) {
                util::Future<int> future;
                int waitSeconds = 30;
                int wakeUpTime = 3;
                util::Thread thread(ClientUtilTest::setExceptionToFuture, &future, &wakeUpTime);
                ASSERT_TRUE(future.waitFor(waitSeconds * 1000));

                try {
                    future.get();
                    FAIL();
                } catch (exception::IException &) {
                    // expect exception here
                }
            }

            void dummyThread(util::ThreadArgs& args) {
            }

            TEST_F (ClientUtilTest, testThreadName) {
                std::string threadName = "myThreadName";
                util::Thread thread(threadName, dummyThread);
                ASSERT_EQ(threadName, thread.getThreadName());
            }

            TEST_F (ClientUtilTest, testThreadJoinAfterThreadExited) {
                std::string threadName = "myThreadName";
                util::CountDownLatch latch(1);
                util::Thread thread(threadName, notifyExitingThread, &latch);
                ASSERT_TRUE(latch.await(2));
                // guarantee that the thread exited
                util::sleep(1);

                // call join after thread exit
                thread.join();
            }

            TEST_F (ClientUtilTest, testCancelJoinItselfFromTheRunningThread) {
                std::string threadName = "myThreadName";
                util::CountDownLatch latch(1);
                util::Thread thread(threadName, cancelJoinFromRunningThread, &latch);
                ASSERT_TRUE(latch.await(1000));
            }

            void sleepyThread(util::ThreadArgs& args) {
                int sleepTime = *(int *)args.arg0;
                args.currentThread->interruptibleSleep(sleepTime);
            }

            TEST_F (ClientUtilTest, testThreadInterruptibleSleep) {
                int sleepTime = 30;
                int wakeUpTime = 3;
                time_t beg = time(NULL);
                util::Thread thread(sleepyThread, &sleepTime);
                util::sleep(wakeUpTime);
                thread.cancel();
                thread.join();
                ASSERT_NEAR((double)(time(NULL) - beg), (double)wakeUpTime , 1);
            }

            TEST_F (ClientUtilTest, testDateConversion) {
                std::string date("2016-04-20");
                util::gitDateToHazelcastLogDate(date);
                ASSERT_EQ("20160420", date);

                date = "NOT_FOUND";
                util::gitDateToHazelcastLogDate(date);
                ASSERT_EQ("NOT_FOUND", date);
            }

            TEST_F (ClientUtilTest, testStrError) {
                #if  defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64)
                int error = WSAEINPROGRESS;
                std::string expectedErrorString("A blocking operation is currently executing.\r\n");
                #else
                int error = EINPROGRESS;
                std::string expectedErrorString("Operation now in progress");
                #endif

                char msg[100];
                const std::string prefix = "testStrError prefix message";
                ASSERT_EQ(0, util::strerror_s(error, msg, 100, prefix.c_str()));
                ASSERT_STREQ((prefix + " " + expectedErrorString).c_str(), msg);

                ASSERT_EQ(0, util::strerror_s(error, msg, prefix.length() + 1, prefix.c_str()));
                ASSERT_STREQ(prefix.c_str(), msg);

                ASSERT_EQ(0, util::strerror_s(error, msg, 100));
                ASSERT_STREQ(expectedErrorString.c_str(), msg);
            }
        }
    }
}

