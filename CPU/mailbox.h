#ifndef MAILBOX_H
#define MAILBOX_H

#include <queue>
#include <unordered_map>
#include <string>
// MailBox struct is intended to implement signal propagation from any stage to any stage. It is safe because only limited uint32_t data are transfered.

enum MailBoxNode {
    IF_Stage,
    ID_Stage,
    EXE_Stage,
    MEM_Stage,
    WB_Stage,
};

class MailBox {
private:
    std::unordered_map<std::string, uint32_t> mailmap;
    const int MAX_MAIL_SIZE = 5;

    int checkBoxSize() {
        return MAX_MAIL_SIZE - mailmap.size();
    }
public:
    bool Send(std::string key, uint32_t value) {
        if (checkBoxSize() < 0) {
            printf("[error] Mailbox is full.\n");
            return false;
        }
        if (mailmap.count(key) > 0) {
            printf("[warning] overwrite existed key\n");
        }
        mailmap[key] = value;
        return true;
    }
    bool Receive(std::string key, uint32_t &value) {
        if (mailmap.count(key) == 0) {
            // No existed key-value
            return false;
        }
        value = mailmap[key];
        mailmap.erase(key);
        return true;
    }
};

// std::unordered_map<std::pair<MailBoxNode, MailBoxNode>, MailBox*> mailboxes;
extern MailBox* mailboxes[5][5]; // mailboxes[Src_Node, Dst_Node] is a MailBox

#endif  // MAILBOX_H
