#include "Global.hpp"

std::string Server::OpMode(t_input *input, std::string params, Channel *channel)
{
    std::string modeMsg = MODE_MSG_NEW;
    if (input->prefix[0] == '+')
    {
        if (params.length() <= 0)
        {
            input->client->SendMsg(OPERATOR_MODE_ERROR);
            return "";
        }
        else
        {
            Client* target = get_ClientByNickname(params);
            if (!target)
            {
                input->client->SendMsg(NO_SUCH_NICK_CHANNEL);
                return "";
            }
            channel->addOperators(target);
            modeMsg = modeMsg + "+o " + params;
        }
        return (modeMsg);
    }
    else if (input->prefix[0] == '-')
    {
        Client* target = get_ClientByNickname(params);
        if (!target)
        {
            input->client->SendMsg(NO_SUCH_NICK_CHANNEL);
            return "";
        }
        channel->removeOperator(target);
        modeMsg = modeMsg + "-o " + params;
    }
    return  "";
}


std::string Server::InviteOnlyMode(t_input *input, std::string params, Channel *channel)
{
    (void)params;
    (void)input;
    std::string modeMsg = MODE_MSG_NEW;
    if (input->prefix[0] == '+')
    {
        if (!channel->getInvite())
        {
            return "";
        }
        channel->setInviteOnly(true);
        modeMsg = modeMsg + "+i";
    }
    else if (input->prefix[0] == '-')
    {
        channel->setInviteOnly(false);
        modeMsg = modeMsg + "-i";
    }
    return (modeMsg);
}


std::string Server::TopicMode(t_input *input, std::string params, Channel *channel)
{
    (void)params;
    std::string modeMsg = MODE_MSG_NEW;
    if (input->prefix[0] == '+')
    {
        if (channel->getTopicChange())
        {
            return "";
        }
        channel->setTopicChange(false);
        modeMsg = modeMsg + "+t";
    }
    else if (input->prefix[0] == '-')
    {
        channel->setTopicChange(true);
        modeMsg = modeMsg + "-t";
    }
    return (modeMsg);
}


std::string Server::KeyMode(t_input *input, std::string params, Channel *channel)
{
    std::string modeMsg = MODE_MSG_NEW;
    if (input->prefix[0] == '+')
    {
        channel->setPassword(params);
        channel->setPasswordNeeded(true);
        modeMsg = modeMsg + "+k " + params;
    }
    else if (input->prefix[0] == '-')
    {
        if (params == channel->getPassword())
        {
            channel->setPassword("");
            channel->setPasswordNeeded(false);
            modeMsg = modeMsg + "-k " + params;
        }
        else
        {
            input->client->SendMsg(INVALID_PASSWORD_MSG);
            return "";
        }
    }

    return (modeMsg);
}


std::string Server::LimitMode(t_input *input, std::string params, Channel *channel)
{
    std::string modeMsg = MODE_MSG_NEW;
    if (input->prefix[0] == '+')
    {
        std::stringstream ss;
        int limit = atoi(params.c_str());
        if (limit <= 0)
        {
            input->client->SendMsg(INVALID_LIMIT_VALUE_MSG);
            return "";
        }
        channel->setUserLimit(limit);
        ss << limit;
        modeMsg = modeMsg + "+l " + ss.str();
    }
    else if (input->prefix[0] == '-')
    {
        channel->setUserLimit(std::numeric_limits<int>::max());
        modeMsg = modeMsg + "-l";
    }
    return (modeMsg);
}
