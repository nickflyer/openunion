公司新开发的开源导航网站
=========
openunion

下面是github的相关信息：<br />
http://www.worldhello.net/gotgithub/02-join-github/010-account-setup.html<br />

下面是提炼出的简明攻略：<br />
第一：认证<br />
ssh -T git@github.com 认证<br />
如果被拒绝，说明没有把本机的git和github链接在一起，需要设置公钥或者私钥。<br />
（1）安装ssh后，在终端下ssh-keygen命令设置公钥/私钥;<br />
（2）使用ssh-keygen -C "gotgithub@gmail.com" -f ~/.ssh/gotgithub 在~/.ssh目录下创建名为gotgithub的私钥和名为gotgithub.pub的公钥文件;<br />
（3）当生成的公钥/私钥对不在缺省位置（~/.ssh/id_rsa等）时，使用ssh命令连接远程主机时需要使用参数-i <filename>指定公钥/私钥对。或者在配置文件~/.ssh/config中针对相应主机进行设定。例如对于上例创建了非缺省公钥/私钥对~/.ssh/gotgithub，可以在~/.ssh/config配置文件中写入如下配置<br />
  Host github.com
  User git
  Hostname github.com
  PreferredAuthentications publickey
  IdentityFile ~/.ssh/gotgithub
<br />
(4)将~/.ssh/gotgithub.pub文件内容拷贝到github账户中的SSH公钥管理的对话框中。
<br />

第二：clone已有的版本（本地没有文件，从github上获得新的文件）<br />
git clone git@github.com:gotgithub/gotgit.git<br />
cd gotgit<br />
<br />
如果本地最初有文件，则如下所示：<br />
cd ?<br />
git init<br />
git add .<br />
git commit -m "Fixed #3: should be 项目, not 项."  <br />
git remote add origin git@github.com:?/?.git<br />
git push -f origin master<br />

第三：（从本地将文件放往github）<br />
cd **<br />
git add ？ <br />
git commit -m "Fixed #3: should be 项目, not 项."  <br />
git push<br />



第四：(github 上有更新，本地获得github更新)<br />
git pull

第五：(在本地更新，上传更新到github服务器)<br />
git add .  <br />
git commit -a -m"some files" <br />
git push origin master <br />

第六：切换到其他的branch<br />
<br />如果还没有其他分支:<br />git checkout -b yangll<br />git push -u origin yangll<br />
<br />
git checkout  yangll<br />
git pull<br />
各种操作<br />
git  add .<br />
git commit -m "branch yangll."<br />
git push origin yangll<br />
<br />

第七：把各自的文件合并到master<br />
git checkout master<br />
git merge yangll<br />
git push origin master<br />


