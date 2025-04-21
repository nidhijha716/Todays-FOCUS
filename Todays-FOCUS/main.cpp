#include <iostream>
#include <vector>
#include <string>
#include <bits/stdc++.h>
// #include <algorithm>

using namespace std;

struct Job
{
  int id;
  string name;
  int duration;
};
vector<Job> j_vec;
Job job;

struct Profit
{
  Job tasks;
  int deadline;
  int profit;
};
vector<Profit> p_tasks;

struct Priority
{
  Job tasks;
  int deadline;
  int priority;
};
vector<Priority> pr_tasks;

struct Maxjob
{
  Job tasks;
  int deadline;
};
vector<Maxjob> m_tasks;

bool comparison(Profit a, Profit b)
{
  return (a.profit > b.profit);
}

bool compare(Priority a, Priority b)
{
  return (a.priority < b.priority);
}

bool comp(Maxjob a, Maxjob b)
{
  return (a.deadline > b.deadline);
}

void computeLPSArray(const string &pat, vector<int> &lps)
{
  int M = pat.size();
  int len = 0;
  lps[0] = 0;
  int i = 1;

  while (i < M)
  {
    if (pat[i] == pat[len])
    {
      len++;
      lps[i] = len;
      i++;
    }
    else
    {
      if (len != 0)
      {
        len = lps[len - 1];
      }
      else
      {
        lps[i] = 0;
        i++;
      }
    }
  }
}

// Function to convert string to lowercase
string toLowercase(const string &str)
{
  string lowercaseStr = str;
  for (char &c : lowercaseStr)
  {
    if (c >= 'A' && c <= 'Z')
    {
      c = c + ('a' - 'A');
    }
  }
  return lowercaseStr;
}

void KMPSearch(const string &pat)
{
  vector<string> matchingStrings;
  int M = pat.size();
  int N;

  string patternLower = toLowercase(pat);

  vector<int> lps(M);
  computeLPSArray(patternLower, lps);

  for (const Job &job : j_vec)
  {
    string textLower = toLowercase(job.name);
    N = job.name.size();
    int i = 0, j = 0;
    while (i < N)
    {
      if (patternLower[j] == textLower[i])
      {
        j++;
        i++;
      }

      if (j == M)
      {
        matchingStrings.push_back(job.name);
        j = lps[j - 1];
      }
      else if (i < N && patternLower[j] != textLower[i])
      {
        if (j != 0)
          j = lps[j - 1];
        else
          i = i + 1;
      }
    }
  }

  if (matchingStrings.empty())
  {
    cout << "No matching task found!" << endl;
  }
  else
  {
    cout << "Tasks with name pattern matching:" << endl;
    for (const string &str : matchingStrings)
      cout << str << endl;
  }
}

void binpack(int n, int dur)
{
  vector<vector<int>> bins(n);
  vector<int> remainingSpace(n, dur);

  for (const auto &d : j_vec)
  {
    int weight = d.duration;
    int maxSpaceIndex = 0;
    int maxSpace = remainingSpace[0];

    for (int i = 1; i < n; ++i)
    {
      if (remainingSpace[i] > maxSpace)
      {
        maxSpace = remainingSpace[i];
        maxSpaceIndex = i;
      }
    }

    if (maxSpace >= weight)
    {
      bins[maxSpaceIndex].push_back(weight);
      remainingSpace[maxSpaceIndex] -= weight;
    }
    else
    {
      cout << "Task " << d.name << " cannot be alloted to any worker." << endl;
    }
  }

  for (int i = 0; i < n; ++i)
  {
    cout << "Worker " << i + 1 << ": ";
    for (int weight : bins[i])
    {
      cout << weight << " ";
    }
    cout << endl;
  }
}

void robin(queue<pair<string, int>> &t, int n)
{
  for (int s = 1; s <= n; ++s)
  {
    if (t.empty())
    {
      cout << "No tasks remaining." << endl;
      return;
    }
    string c = t.front().first;
    int d = t.front().second;
    // cout << "Slot " << s << ": " << c << endl;
    cout << "start time: " << s - 1 << "--" << "end time: " << s << "   task: " << c << " " << endl;
    --d;
    if (d <= 0)
    {
      t.pop();
    }
    else
    {
      t.push(make_pair(c, d));
      t.pop();
    }
  }
}

void round_robin(queue<pair<string, int>> &ts, int workers, int dur)
{
  vector<string> wts(workers, "");
  vector<int> rwts(workers, dur);

  while (!ts.empty())
  {
    string cur_task = ts.front().first;
    int task_dur = ts.front().second;
    ts.pop();

    int max_avail_time = -1;
    int worker_ind = -1;
    for (int i = 0; i < workers; ++i)
    {
      if (rwts[i] >= task_dur && rwts[i] > max_avail_time)
      {
        max_avail_time = rwts[i];
        worker_ind = i;
      }
    }

    if (worker_ind != -1)
    {
      wts[worker_ind] += cur_task + " ";
      rwts[worker_ind] -= task_dur;
      cout << "Assigned task '" << cur_task << "' to Worker " << worker_ind + 1 << endl;
    }
    else
    {
      cout << "Task '" << cur_task << "' could not be assigned to any worker." << endl;
    }
  }
}

void priority_sched(int num)
{
  Priority job_new2;
  for (int i = 0; i < num; i++)
  {

    cout << "Enter the required data for the task " << i + 1 << ": \n";
    job_new2.tasks = j_vec[i];
    cout << " Deadline: ";
    cin >> job_new2.deadline;
    cout << "Priority (priority-1 being the highest): ";
    cin >> job_new2.priority;
    pr_tasks.push_back(job_new2);
  }

  sort(pr_tasks.begin(), pr_tasks.end(), compare);
  vector<int> slot(24, -1);

  for (int i = 0; i < num; i++)
  {
    int dead = pr_tasks[i].deadline;
    int dur = pr_tasks[i].tasks.duration;

    // Find the latest available slot just before the deadline
    int index = dead - 1;
    while (index >= 0 && slot[index] != -1)
    {
      index--;
    }
    // If there's no available slot, skip this job
    if (index < 0)
    {
      continue;
    }

    // Check if there are enough contiguous slots available
    bool found = true;
    for (int j = index; j >= max(0, index - dur + 1); j--)
    {
      if (slot[j] != -1)
      {
        found = false;
        break;
      }
    }

    // If contiguous slots are available, schedule the job
    if (found)
    {
      for (int j = index; j >= max(0, index - dur + 1); j--)
      {
        slot[j] = i;
      }
    }
  }
  cout << "Scheduled tasks are as follows: " << endl;
  for (int i = 0; i < 24; i++)
  {
    if (slot[i] != -1)
    {

      cout << "start time: " << i << "--" << "end time: " << i + 1 << "   task: " << pr_tasks[slot[i]].tasks.id << " " << endl;
    }
  }
}

void maxjob_sched(int num)
{
  Maxjob job_new3;
  for (int i = 0; i < num; i++)
  {

    cout << "Enter the required data for the task " << i + 1 << ": \n";
    job_new3.tasks = j_vec[i];
    cout << "Deadline: ";
    cin >> job_new3.deadline;
    m_tasks.push_back(job_new3);
  }
  int ans = 0, hrs = 0;
  sort(m_tasks.begin(), m_tasks.end(), comp);
  vector<int> slot(24, -1);

  for (int i = 0; i < num; i++)
  {
    int dead = m_tasks[i].deadline;
    int dur = m_tasks[i].tasks.duration;

    // Find the latest available slot just before the deadline
    int index = dead - 1;
    while (index >= 0 && slot[index] != -1)
    {
      index--;
    }
    // If there's no available slot, skip this job
    if (index < 0)
    {
      continue;
    }

    // Check if there are enough contiguous slots available
    bool found = true;
    for (int j = index; j >= max(0, index - dur + 1); j--)
    {
      if (slot[j] != -1)
      {
        found = false;
        break;
      }
    }

    // If contiguous slots are available, schedule the job
    if (found)
    {
      for (int j = index; j >= max(0, index - dur + 1); j--)
      {
        slot[j] = i;
      }
      ans++;
    }
  }

  cout << "Scheduled tasks are as follows: " << endl;
  for (int i = 0; i < 24; i++)
  {
    if (slot[i] != -1)
    {

      cout << "start time: " << i << "--" << "end time: " << i + 1 << "   task: " << m_tasks[slot[i]].tasks.id << " " << endl;
      hrs++;
    }
  }

  cout << "maximum jobs scheduled are: " << ans << endl;
  cout << "maximum hours of working scheduled are: " << hrs << endl;
}

void profit_sched(int num)
{
  Profit job_new;
  for (int i = 0; i < num; i++)
  {
    cout << "Enter the required data for the task " << i + 1 << ": \n";
    job_new.tasks = j_vec[i];
    cout << " Deadline: ";
    cin >> job_new.deadline;
    cout << "Profit: ";
    cin >> job_new.profit;
    p_tasks.push_back(job_new);
  }
  int ans = 0;
  sort(p_tasks.begin(), p_tasks.end(), comparison);
  vector<int> slot(24, -1);

  for (int i = 0; i < num; i++)
  {
    int dead = p_tasks[i].deadline;
    int dur = p_tasks[i].tasks.duration;

    // Find the latest available slot just before the deadline
    int index = dead - 1;
    while (index >= 0 && slot[index] != -1)
    {
      index--;
    }
    // If there's no available slot, skip this job
    if (index < 0)
    {
      continue;
    }

    // Check if there are enough contiguous slots available
    bool found = true;
    for (int j = index; j >= max(0, index - dur + 1); j--)
    {
      if (slot[j] != -1)
      {
        found = false;
        break;
      }
    }

    // If contiguous slots are available, schedule the job
    if (found)
    {
      for (int j = index; j >= max(0, index - dur + 1); j--)
      {
        slot[j] = i;
      }
      ans += p_tasks[i].profit;
    }
  }
  cout << "Scheduled tasks are as follows: " << endl;
  for (int i = 0; i < 24; i++)
  {
    if (slot[i] != -1)
    {

      cout << "start time: " << i << "--" << "end time: " << i + 1 << "   task: " << p_tasks[slot[i]].tasks.id << " " << endl;
    }
  }

  cout << "maximum profit is: " << ans;
}

void add_tasks(int num)
{

  for (int i = 0; i < num; i++)
  {

    cout << "Enter the details of the task: " << i + 1 << ": ";
    job.id = i + 1;
    cout << "Name: ";
    cin >> job.name;
    cout << "Duration: ";
    cin >> job.duration;
    j_vec.push_back(job);
  }
}

void choice_individual(int num)
{
  cout << "Choose the criteria for scheduling tasks: ";
  int ch;
  cout << "\n\t\t\t\t\t***SUB MENU***\t\t\t\t\t\n";
  cout << "\n\nEnter choice:\n\n1. MAXIMUM PROFIT \n2. HIGHEST PRIORITY  \n3. MAXIMUM JOB SCHEDULED \n4. ROUND ROBIN \n5. EXIT\n";
  cin >> ch;
  switch (ch)
  {
  case 1:
  {
    cout << " SCHEDULING JOBS BASED ON THE MAXIMUM PROFIT: \n\n";
    profit_sched(num);

    break;
  }

  case 2:
  {
    cout << " SCHEDULING THE JOBS BASED ON THE HIGHEST PRIORITY: \n\n";
    priority_sched(num);
    break;
  }
  case 3:
  {
    cout << " SCHEDULING THE JOBS BASED ON THE MAXIMUM JOB SCHEDULED: \n\n";
    maxjob_sched(num);
    break;
  }

  case 4:
  {
    cout << " SCHEDULING THE JOB BASED ON THE ROUND ROBIN: \n\n";
    queue<pair<string, int>> tQueue;
    for (size_t i = 0; i < j_vec.size(); ++i)
    {
      Job task = j_vec[i];
      tQueue.push(make_pair(task.name, task.duration));
    }

    robin(tQueue, 24);
    break;
  }

  case 5:
  {
    // exiting the program
    cout << "\n\t\t\tTHANK YOU!!!\n";
    exit(0);
  }
  default:
    cout << "Invalid Choice!!! ";
    break;
  }
}

void choice_company(int num, int n, int dur)
{
  cout << "Choose the criteria for scheduling tasks: ";
  int ch2;
  cout << "\n\t\t\t\t\t***SUB MENU***\t\t\t\t\t\n";
  cout << "\n\nEnter choice:\n\n1. JOB DISTRIBUTION \n2. ROUND ROBIN DISTRIBUTION \n3. EXIT\n";
  cin >> ch2;
  switch (ch2)
  {
  case 1:
  {
    cout << " JOB DISTRIBUTION: \n\n";
    binpack(n, dur);

    break;
  }

  case 2:
  {
    cout << " ROUND ROBIN DISTRIBUTION: \n\n";
    queue<pair<string, int>> ts;
    for (const auto &j : j_vec)
    {
      ts.push(make_pair(j.name, j.duration));
    }
    round_robin(ts, n, dur);

    break;
  }

  case 3:
    // exiting the program
    cout << "\n\t\t\tTHANK YOU!!!\n";
    exit(0);

  default:
    cout << "Invalid Choice!!! ";
    break;
  }
}

// MAIN FUNCTION DRIVER CODE
int main()
{
  cout << "\n\n\n\t\t\t\t    ^^^^^^^^^^^^^^^^^^ WELCOME ^^^^^^^^^^^^^^^^^^^\n";
  int choice;

  cout << "\n\t\t\t\t\t***MENU***\t\t\t\t\t\n";
  cout << "\n\nEnter choice:\n\n1. INDIVIDUAL \n2. COMPANY \n3. EXIT\n";
  cin >> choice;
  int num;
  char x;
  switch (choice)
  {

  case 1:
    cout << "\t\tLOGGING IN AS AN INDIVIDUAL: \n\n";

    cout << "Enter the number of tasks: ";
    cin >> num;
    add_tasks(num);
    choice_individual(num);

    cout << "\nSEARCH TASK: [Y/N]\n";
    cin >> x;

    if (x == 'y' || 'Y')
    {
      string pat;

      cout << "Enter the task name to search : ";
      cin >> pat;

      KMPSearch(pat);
      break;
    }
    else

      break;

  case 2:

    cout << "\t\tLOGGING IN AS A COMPANY: \n\n";

    cout << "Enter the number of tasks: ";
    cin >> num;
    add_tasks(num);
    int n, dur;
    cout << "Enter the number of workers to allot these tasks to: ";
    cin >> n;
    cout << "Enter the work duration for each worker: ";
    cin >> dur;
    choice_company(num, n, dur);

    cout << "\nSEARCH TASK: [Y/N]\n";
    cin >> x;

    if (x == 'y' || 'Y')
    {
      string pat;

      cout << "Enter the task name to search : ";
      cin >> pat;

      KMPSearch(pat);
      break;
    }
    else

      break;

  case 3:
    cout << "\n\t\t\tTHANK YOU!!!\n";
    exit(0);

  default:
    cout << "Invalid Choice!!! ";
    break;
  }

  return 0;
}